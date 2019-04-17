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
#define DEFAULT_TIMEOUT "1000" // milliseconds
#define DEFAULT_LOG_DIRECTORY "./logs"
#define DEFAULT_LOG_FILENAME "basics"
#define DEFAULT_LOG_LEVEL "info"
#define DEFAULT_LOG_MAX_SIZE "1000" // bytes
#define DEFAULT_LOG_MAX_FILES "5"

#include "server/client_data.hpp"
#include "server/server_engine.hpp"
#include "config/config_wrapper.hpp"
#include "constants/request_code.hpp"
#include "constants/response_packet.hpp"
#include "nlohmann/json.hpp"
#include "plog/include/plog/Log.h"
#include "plog/include/plog/Appenders/ColorConsoleAppender.h"

#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <thread>

namespace server {

ResponsePacket ServerEngine::initServer(std::string path) {
	if (state_ != State::INSTANCIED) {
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_INVALID_STATE, .err_server_description = "Server already initialized" };
		return response_packet;
	}

	socket_ = new ServerTCPSocket();
	config_.init(path);

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

	// launch engine
	LOG_INFO << "Server launched";
	state_ = State::INITIALIZED;
	ResponsePacket response_packet;
	return response_packet;
}

ResponsePacket ServerEngine::startListening(const char* ip, const char* port) {
	bool socket_response;

	if (state_ != State::INITIALIZED && state_ != State::DISCONNECTED) {
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_INVALID_STATE, .err_server_description = "Server invalid state" };
		return response_packet;
	}

	// start the server
	socket_response = socket_->startServer(ip, port);
	if (!socket_response) {
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Failed to start server" };
		return response_packet;
	}
	state_ = State::STARTED;
	stop_ = false;
	LOG_INFO << "Start listening on IP " << ip << " and port " << port;

	// launch a thread to handle incoming connections
	std::thread thr(&ServerEngine::handleConnections, this);
	std::swap(thr, connection_thread_);

	ResponsePacket response_packet;
	return response_packet;
}

ResponsePacket ServerEngine::handleConnections() {
	bool socket_response;
	int default_timeout = std::atoi(config_.getValue("timeout", DEFAULT_TIMEOUT).c_str());

	while (!stop_.load()) {
		SOCKET client_socket = INVALID_SOCKET;

		// accept incoming connection
		socket_response = socket_->acceptConnection(&client_socket, default_timeout);
		if (!socket_response) {
			ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Connection with client failed" };
			return response_packet;
		}

		// launch a thread to perform the connection handshake
		std::future<ResponsePacket> fut = std::async(std::launch::async, &ServerEngine::connectionHandshake, this, client_socket);
	}

	ResponsePacket response_packet;
	return response_packet;
}

ResponsePacket ServerEngine::connectionHandshake(SOCKET client_socket) {
	bool response;
	char client_name[DEFAULT_BUFLEN];

	response = socket_->receivePacket(client_socket, client_name);
	if (!response) {
		LOG_INFO << "Handshake with client failed";
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Network error on receive" };
		return response_packet;
	}

	ClientData* client = new ClientData(client_socket, ++next_client_id_, client_name);
	clients_.insert(std::make_pair(client->getId(), client));
	LOG_INFO << "Client connected [id:" << client->getId() << "][name:" << client->getName() << "]";
	if (notifyConnectionAccepted_ != 0) notifyConnectionAccepted_(client->getId(), client->getName().c_str());

	ResponsePacket response_packet;
	return response_packet;
}

ResponsePacket ServerEngine::handleRequest(int id_client, RequestCode request, std::string data, DWORD timeout) {
	if (state_ != State::STARTED) {
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_INVALID_STATE, .err_server_description = "Server must be started" };
		return response_packet;
	}

	if (clients_.find(id_client) == clients_.end()) {
		LOG_DEBUG << "Failed to retrieve client "
		          << "[id_client:" << id_client << "][request:" << requestCodeToString(request) << "]";
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_CLIENT_CLOSED, .err_server_description = "Client closed or not found" };
		return response_packet;
	}

	SOCKET client_socket = clients_.at(id_client)->getSocket();

	nlohmann::json j;
	j["request"] = request;
	j["data"] = data;
	j["timeout"] = timeout;

	DWORD socket_timeout = std::atoi(config_.getValue("timeout", DEFAULT_TIMEOUT).c_str());

	// sends async request to client
	std::future<ResponsePacket> fut = std::async(std::launch::async, &asyncRequest, this, client_socket, j.dump(), socket_timeout);
	// blocks until the timeout has elapsed or the result became available
	if (fut.wait_for(std::chrono::milliseconds(socket_timeout)) == std::future_status::timeout) {
		// thread has timed out
		LOG_DEBUG << "Response time from client has elapsed "
		          << "[client_socket:" << client_socket << "][request:" << j.dump << "[timeout:" << timeout << "]";
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_TIMEOUT, .err_server_description = "Request time elapsed" };
		return response_packet;
	}
	return fut.get();
}

ResponsePacket ServerEngine::asyncRequest(SOCKET client_socket, std::string to_send, DWORD timeout) {
	bool socket_response;
	char recvbuf[DEFAULT_BUFLEN];

	socket_response = socket_->sendPacket(client_socket, to_send.c_str());
	if (!socket_response) {
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Network error on send request" };
	}
	LOG_INFO << "Data sent to client: " << to_send.c_str();

	socket_response = socket_->receivePacket(client_socket, recvbuf);
	if (!socket_response) {
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Network error on receive" };
		return response_packet;
	}

	LOG_INFO << "Data received: " << recvbuf;
	nlohmann::json response = nlohmann::json::parse(recvbuf); // parses response to json object
	ResponsePacket response_packet = response.get<ResponsePacket>();
	return response_packet;
}

ResponsePacket ServerEngine::listClients() {
	if (state_ != State::STARTED) {
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_INVALID_STATE, .err_server_description = "Server must be started" };
		return response_packet;
	}

	std::string output = "Clients connected: " +  std::to_string(clients_.size()) + "|";
	for (const auto &p : clients_) {
		output += std::to_string(p.second->getId()) + "|" + p.second->getName() + "|";
	}
	ResponsePacket response_packet = { .response = output };
	return response_packet;
}

ResponsePacket ServerEngine::stopAllClients() {
	if (state_ != State::STARTED) {
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_INVALID_STATE, .err_server_description = "Server must be started" };
		return response_packet;
	}

	stop_ = true; // stop active threads
	socket_->closeServer();
	connection_thread_.join();

	for (const auto &p : clients_) {
		stopClient(p.first);
	}

	state_ = State::DISCONNECTED;
	ResponsePacket response_packet;
	return response_packet;
}

ResponsePacket ServerEngine::stopClient(int id_client) {
	if (state_ != State::STARTED) {
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_INVALID_STATE, .err_server_description = "Server must be started" };
		return response_packet;
	}

	SOCKET client_socket = clients_.at(id_client)->getSocket();
	ResponsePacket response_packet = handleRequest(id_client, REQ_DISCONNECT);
	if (response_packet.err_server_code  < 0) {
		return response_packet;
	}

	int retval = shutdown(client_socket, SD_SEND);
	if (retval == SOCKET_ERROR) {
		LOG_DEBUG << "Failed to shutdown client "
				  << "[client_socket:" << client_socket << "][how:" << SD_SEND << "]";
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Client shutdown failed" };
		return response_packet;
	}

	closesocket(client_socket);
	delete clients_.at(id_client);
	clients_.erase(id_client);
	return response_packet;
}

} /* namespace server */
