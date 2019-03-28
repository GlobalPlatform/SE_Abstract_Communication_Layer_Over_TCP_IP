/*
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
*/

#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN

#define DEFAULT_BUFLEN 1024 * 64
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT "66611"
#define DEFAULT_TIMEOUT "1000" // milliseconds

#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "nlohmann/json.hpp"
#include "plog/include/plog/Log.h"
#include "plog/include/plog/Appenders/ColorConsoleAppender.h"

#include "server/client_data.h"
#include "server/server_engine.h"
#include "config/config_wrapper.h"
#include "constants/request_code.h"
#include "constants/response_packet.h"

namespace server {

ResponsePacket ServerEngine::initServer(std::string path) {
	config.init(path);

	// setup logger
	std::string logfile_path = config.getValue("log_path");
	std::string log_level = config.getValue("log_level", "info");
	static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
	if (log_level.compare("debug") == 0) {
		plog::init(plog::debug, logfile_path.c_str()).addAppender(&consoleAppender);
	} else {
		plog::init(plog::info, logfile_path.c_str()).addAppender(&consoleAppender);
	}

	// launch engine
	LOG_INFO << "Server launched";
	ResponsePacket response_packet;
	return response_packet;
}

ResponsePacket ServerEngine::startListening() {
	std::string ip = config.getValue("ip", DEFAULT_IP);
	std::string port = config.getValue("port", DEFAULT_PORT);

	LOG_INFO << "Start listening";
	WSADATA wsaData;
	int retval;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// initializes Winsock
	retval = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (retval != 0) {
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "WSAStartup failed" };
		return response_packet;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	retval = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
	if (retval != 0) {
		WSACleanup();
		LOG_DEBUG << "Failed to call getaddrinfo() "
				  << "[ip:" << ip << "][port:" << port << "]";
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Can't resolve server address and port" };
		return response_packet;
	}

	// Create a SOCKET for connecting to server
	listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listen_socket == INVALID_SOCKET) {
		LOG_DEBUG << "Failed to call socket() "
		          << "[ip:" << ip << "][port:" << port << "][WSAError:" << WSAGetLastError() << "]";
		freeaddrinfo(result);
		WSACleanup();
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Socket creation failed" };
		return response_packet;
	}

	// Setup the TCP listening socket
	retval = bind(listen_socket, result->ai_addr, (int) result->ai_addrlen);
	if (retval == SOCKET_ERROR) {
		LOG_DEBUG << "Failed to call bind() "
		          << "[ip:" << ip << "][port:" << port << "][WSAError:" << WSAGetLastError() << "]";
		freeaddrinfo(result);
		closesocket(listen_socket);
		WSACleanup();
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Bind failed" };
		return response_packet;
	}

	freeaddrinfo(result);

	retval = listen(listen_socket, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		LOG_DEBUG << "Failed to call listen() "
		          << "[ip:" << ip << "][port:" << port << "][WSAError:" << WSAGetLastError() << "]";
		closesocket(listen_socket);
		WSACleanup();
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Listen failed" };
		return response_packet;
	}

	std::thread thr(&ServerEngine::handleConnections, this, listen_socket);
	std::swap(thr, connection_thread);

	ResponsePacket response_packet;
	return response_packet;
}

ResponsePacket ServerEngine::handleConnections(SOCKET listen_socket) {
	int connection_timeout = std::atoi(config.getValue("timeout", DEFAULT_TIMEOUT).c_str());

	while (!stop_flag.load()) {
		SOCKET client_socket = INVALID_SOCKET;

	    timeval timeout = {1, 0}; // select will return each second
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(listen_socket, &fds);
		select(listen_socket, &fds, NULL, NULL, &timeout);

		if (FD_ISSET(listen_socket, &fds)) {
			client_socket = accept(listen_socket, NULL, NULL);
			if (client_socket == INVALID_SOCKET) {
				LOG_DEBUG << "Failed to call accept() "
				          << "[listen_socket:" << listen_socket << "][WSAError:" << WSAGetLastError() << "]";
				ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Connection with client failed" };
				return response_packet;
			}

			std::future<ResponsePacket> fut = std::async(std::launch::async, &ServerEngine::connectionHandshake, this, client_socket);
			// blocks until the timeout has elapsed or the result becomes available.
			if (fut.wait_for(std::chrono::milliseconds(connection_timeout)) == std::future_status::timeout) {
				// connection has timed out
				LOG_DEBUG << "Connection thread with client has elapsed "
				          << "[client_socket:" << client_socket << "][connection_timeout:" << connection_timeout << "]";
			} else {
				fut.get();
			}
		}
	}

	ResponsePacket response_packet;
	return response_packet;
}

ResponsePacket ServerEngine::connectionHandshake(SOCKET client_socket) {
	std::string timeout = config.getValue("timeout", DEFAULT_TIMEOUT);

	char recvbuf[DEFAULT_BUFLEN];
	setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(timeout));
	int retval = recv(client_socket, recvbuf, DEFAULT_BUFLEN, 0);
	if (retval == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAETIMEDOUT) {
			ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Network error on receive" };
			return response_packet;
		} else {
			ResponsePacket response_packet = {.response = "KO",  .err_server_code = ERR_TIMEOUT, .err_server_description = "Connection time elapsed" };
			return response_packet;
		}
	}

	if (retval > 0) {
		recvbuf[retval] = '\0';
		ClientData* client = new ClientData(client_socket, ++next_client_id, recvbuf);
		clients.insert(std::make_pair(client->getId(), client));
		LOG_INFO << "Client connected [id:" << client->getId() << "][name:" << client->getName() << "]";
		if (notifyConnectionAccepted != 0) notifyConnectionAccepted(client->getId(), client->getName().c_str());
		ResponsePacket response_packet;
		return response_packet;
	} else {
		LOG_DEBUG << "Failed to receive data from server, connection reset by peer";
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Network error no data received" };
		return response_packet;
	}
}

ResponsePacket ServerEngine::handleRequest(int id_client, RequestCode request, std::string data, DWORD timeout) {
	if (clients.find(id_client) == clients.end()) {
		LOG_DEBUG << "Failed to retrieve client "
		          << "[id_client:" << id_client << "][request:" << requestCodeToString(request) << "]";
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_CLIENT_CLOSED, .err_server_description = "Client closed or not found" };
		return response_packet;
	}

	SOCKET client_socket = clients.at(id_client)->getSocket();

	nlohmann::json j;
	j["request"] = request;
	j["data"] = data;
	j["timeout"] = timeout;

	// sends async request to client
	std::future<ResponsePacket> fut = std::async(std::launch::async, &asyncRequest, this, client_socket, j.dump(), timeout);

	// blocks until the timeout has elapsed or the result becomes available.
	if (fut.wait_for(std::chrono::milliseconds(timeout)) == std::future_status::timeout) {
		// thread has timed out
		LOG_DEBUG << "Response time from client has elapsed "
		          << "[client_socket:" << client_socket << "][request:" << j.dump << "[timeout:" << timeout << "]";
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_TIMEOUT, .err_server_description = "Request time elapsed" };
		return response_packet;
	}

	return fut.get();
}

ResponsePacket ServerEngine::asyncRequest(SOCKET client_socket, std::string to_send, DWORD timeout) {
	const char* to_send_char = to_send.c_str();
	LOG_INFO << "Data sent to client: " << to_send_char;
	int retval = send(client_socket, to_send_char, strlen(to_send_char), 0);
	if (retval == SOCKET_ERROR) {
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Network error on send request" };
		return response_packet;
	}

	char recvbuf[DEFAULT_BUFLEN];

	setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(tm));
	retval = recv(client_socket, recvbuf, DEFAULT_BUFLEN, 0);
	if (retval == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) {
			ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_TIMEOUT, .err_server_description = "Request time elapsed" };
			return response_packet;
		} else {
			LOG_DEBUG << "Failed to receive data from client "
					  << "[client_socket:" << client_socket << "][recvbuf:" << recvbuf << "][size:" << DEFAULT_BUFLEN << "][flags:" << NULL << "][WSAError:" << WSAGetLastError() << "]";
			ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Network error on receive" };
			return response_packet;
		}
	}

	// processes client's response
	if (retval > 0) {
		recvbuf[retval] = '\0';
		LOG_INFO << "Data received: " << recvbuf;
		nlohmann::json response = nlohmann::json::parse(recvbuf); // parses response to json object
		ResponsePacket response_packet = response.get<ResponsePacket>();
		return response_packet;
	} else {
		LOG_DEBUG << "Client unreachable "
				  << "[client_socket:" << client_socket << "]";
		ResponsePacket response_packet = { .response = "KO", .err_server_code = ERR_NETWORK, .err_server_description = "Network error no data received" };
		return response_packet;
	}
}

ResponsePacket ServerEngine::listClients() {
	std::string output = "Clients connected: " +  std::to_string(clients.size()) + "|";
	for (const auto &p : clients) {
		output += std::to_string(p.second->getId()) + "|" + p.second->getName() + "|";
	}
	ResponsePacket response_packet = { .response = output };
	return response_packet;
}

ResponsePacket ServerEngine::stopAllClients() {
	stop_flag = true; // stop active threads
	connection_thread.join();

	for (const auto &p : clients) {
		stopClient(p.first);
		delete(p.second);
	}

	closesocket(listen_socket);
	WSACleanup();

	ResponsePacket response_packet;
	return response_packet;
}

ResponsePacket ServerEngine::stopClient(int id_client) {
	SOCKET client_socket = clients.at(id_client)->getSocket();

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

	return response_packet;
}

} /* namespace server */
