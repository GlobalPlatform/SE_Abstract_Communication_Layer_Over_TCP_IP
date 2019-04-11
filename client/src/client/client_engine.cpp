/*********************************************************************************
 Copyright 2017 GlobalPlatform, Inc.

 Licensed under the GlobalPlatform/Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

https://github.com/GlobalPlatform/SE-test-IP-connector/blob/master/Charter%20and%20Rules%20for%20the%20SE%20IP%20connector.docx

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*********************************************************************************/

#define DEFAULT_BUFLEN 1024 * 64
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT "66611"
#define DEFAULT_NAME "default_name"
#define DEFAULT_LOG_DIRECTORY "./logs"
#define DEFAULT_LOG_FILENAME "basics"
#define DEFAULT_LOG_LEVEL "info"
#define DEFAULT_LOG_MAX_SIZE "1000" // bytes
#define DEFAULT_LOG_MAX_FILES "5"

#include <fstream>
#include <future>
#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <thread>

#include "nlohmann/json.hpp"
#include "plog/include/plog/Log.h"
#include "plog/include/plog/Appenders/ColorConsoleAppender.h"
#include "plog/include/plog/Appenders/RollingFileAppender.h"

#include <client/client_engine.hpp>
#include <client/client_tcp_socket.hpp>
#include <config/config_wrapper.hpp>
#include <constants/request_code.hpp>
#include <constants/response_packet.hpp>
#include <terminal/flyweight_terminal_factory.hpp>
#include <terminal/terminals/terminal.hpp>
#include <terminal/terminals/utils/type_converter.hpp>

namespace client {

ClientEngine::~ClientEngine() {
	delete terminal_;
	delete socket_;
}

ResponsePacket ClientEngine::initClient(std::string path, FlyweightTerminalFactory available_terminals, FlyweightRequests available_requests) {
	config_.init(path);
	terminal_ = available_terminals.getFactory(config_.getValue("terminal"))->create();
	requests_ = available_requests;
	socket_ = new ClientTCPSocket();

	// setup logger
	std::string log_directory = config_.getValue("log_directory", DEFAULT_LOG_DIRECTORY);
	std::string log_filename = config_.getValue("log_filename", DEFAULT_LOG_FILENAME);
	CreateDirectory(log_directory.c_str(), NULL);
	std::string log_level = config_.getValue("log_level", DEFAULT_LOG_LEVEL);
	int log_max_size = std::stoi(config_.getValue("log_max_size", DEFAULT_LOG_MAX_SIZE));
	int log_max_files = std::stoi(config_.getValue("log_max_files", DEFAULT_LOG_MAX_FILES));
	static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
	std::string log_path = log_directory + "/" + log_filename;
	if (log_level.compare("debug") == 0) {;
		plog::init(plog::debug, log_path.c_str(), log_max_size, log_max_files).addAppender(&consoleAppender);
	} else {
		plog::init(plog::info, log_path.c_str(), log_max_size, log_max_files).addAppender(&consoleAppender);
	}

	// launch terminal
	ResponsePacket response_packet;
	response_packet = terminal_->init();
	if (response_packet.err_terminal_code != SUCCESS || response_packet.err_card_code != SUCCESS) {
		LOG_INFO << "Client unable to be initialized";
		return response_packet;
	}
	initialized_ = true;

	LOG_INFO << "Client initialized successfully";
	return response_packet;
}

ResponsePacket ClientEngine::loadAndListReaders() {
	ResponsePacket response;
	if (!initialized_.load()) {
		ResponsePacket response_packet = { .response = "KO", .err_client_code = ERR_INVALID_STATE, .err_client_description = "Client must be initialized correctly" };
		return response_packet;
	}
	return terminal_->loadAndListReaders();
}

ResponsePacket ClientEngine::connectClient(const char* reader, const char* ip, const char* port) {
	ResponsePacket packet;
	bool response;

	if (!initialized_.load()) {
		ResponsePacket response_packet = { .response = "KO", .err_client_code = ERR_INVALID_STATE, .err_client_description = "Failed to connect: client must be initialized correctly" };
		return response_packet;
	}

	if (connected_.load()) {
		ResponsePacket response_packet = { .response = "KO", .err_client_code = ERR_INVALID_STATE, .err_client_description = "Failed to connect: client is already connected" };
		return response_packet;
	}

	LOG_INFO << "Client trying to connect on IP " << ip << " port " << port;
	response = socket_->initClient(ip, port);
	if (!response) {
		ResponsePacket response_packet = { .response = "KO", .err_client_code = ERR_NETWORK, .err_client_description = "Failed to connect: initialization failed" };
		return response_packet;
	}

	response = socket_->connectClient();
	if (!response) {
		ResponsePacket response_packet = { .response = "KO", .err_client_code = ERR_NETWORK, .err_client_description = "Failed to connect: check the server" };
		return response_packet;
	}

	std::string name = config_.getValue("name", DEFAULT_NAME).append(" - ").append(reader);
	socket_->sendData(name.c_str());
	Sleep(1000);

	packet = terminal_->connect(reader);
	if (packet.err_card_code < 0 || packet.err_terminal_code < 0) {
		socket_ ->closeClient();
		return packet;
	}

	connected_ = true;
	LOG_INFO << "Client connected on IP " << ip << " port " << port;

	std::thread thr(&ClientEngine::waitingRequests, this);
	thr.detach();

	return packet;
}

ResponsePacket ClientEngine::disconnectClient() {
	if (!connected_.load()) {
		LOG_DEBUG << "Failed to disconnect: not connected yet";
		ResponsePacket response_packet = { .response = "KO", .err_client_code = ERR_INVALID_STATE, .err_client_description = "Failed to disconnect: not connected yet" };
		return response_packet;
	}

	connected_ = false;
	socket_->closeClient();
	ResponsePacket response = terminal_->disconnect();
	if (notifyConnectionLost_ != 0) notifyConnectionLost_("End of connection");
	LOG_INFO << "Client disconnected successfully";
	return response;
}

ResponsePacket ClientEngine::waitingRequests() {
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	bool response;

	LOG_INFO << "Client ready to process incoming requests";

	// receives until the server closes the connection
	while (connected_.load()) {
		response = socket_->receiveData(recvbuf, recvbuflen);
		if (response) {
			std::async(std::launch::async, &ClientEngine::handleRequest, this, recvbuf);
		} else {
			disconnectClient();
		}
	}
	LOG_INFO << "Client not waiting for requests";

	ResponsePacket response_packet;
	return response_packet;
}

ResponsePacket ClientEngine::handleRequest(std::string request) {
	nlohmann::json response;
	ResponsePacket response_packet;

	nlohmann::json j = nlohmann::json::parse(request);
	IRequest* request_handler = requests_.getRequest(j["request"]);
	unsigned long int length;
	unsigned char* command = utils::stringToUnsignedChar(j["data"].get<std::string>(), &length);
	std::future<ResponsePacket> future = std::async(std::launch::async, &IRequest::run, request_handler, terminal_, this, command, length);

	// blocks until the timeout has elapsed or the result becomes available
	if (future.wait_for(std::chrono::milliseconds(j["timeout"])) == std::future_status::timeout) {
		LOG_DEBUG << "Response time from terminal has elapsed "
				  << "[request:" << request << "]";
		ResponsePacket response_packet = { .response = "KO", .err_client_code = ERR_TIMEOUT, .err_client_description = "Request terminal time elapsed" };
		response = response_packet;
	} else {
		response = future.get();
	}

	std::string to_send = response.dump();
	if (socket_->sendData(to_send.c_str())) {
		LOG_INFO << "Data sent to server: " << to_send;
		if (notifyResponseSent_ != 0) notifyResponseSent_(to_send.c_str());
	} else {
		ResponsePacket response_packet = { .response = "KO", .err_client_code = ERR_NETWORK, .err_client_description = "Network error on send response" };
		return response_packet;
	}

	return response_packet;
}

} /* namespace client */
