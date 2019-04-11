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

#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN

#include "server/server_tcp_socket.h"
#include "plog/include/plog/Log.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

bool ServerTCPSocket::start_server(const char* ip, const char* port) {
	int retval = 0;

	// initializes Winsock
	retval = WSAStartup(MAKEWORD(2, 2), &wsaData_);
	if (retval != 0) {
		LOG_DEBUG << "Failed to call WSAStartup()";
		return false;
	}

	ZeroMemory(&hints_, sizeof(hints_));
	hints_.ai_family = AF_INET;
	hints_.ai_socktype = SOCK_STREAM;
	hints_.ai_protocol = IPPROTO_TCP;
	hints_.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	retval = getaddrinfo(ip, port, &hints_, &result_);
	if (retval != 0) {
		WSACleanup();
		LOG_DEBUG << "Failed to call getaddrinfo() " << "[ip:" << ip << "][port:" << port << "]";
		return false;
	}

	// Create a SOCKET for connecting to server
	server_socket_ = socket(result_->ai_family, result_->ai_socktype, result_->ai_protocol);
	if (server_socket_ == INVALID_SOCKET) {
		LOG_DEBUG << "Failed to call socket() " << "[ip:" << ip << "][port:" << port << "][WSAError:" << WSAGetLastError() << "]";
		freeaddrinfo(result_);
		WSACleanup();
		return false;
	}

	// Setup the TCP listening socket
	retval = bind(server_socket_, result_->ai_addr, (int) result_->ai_addrlen);
	if (retval == SOCKET_ERROR) {
		LOG_DEBUG << "Failed to call bind() " << "[ip:" << ip << "][port:" << port << "][WSAError:" << WSAGetLastError() << "]";
		close_server();
		return false;
	}
	freeaddrinfo(result_);

	retval = listen(server_socket_, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		LOG_DEBUG << "Failed to call listen() " << "[ip:" << ip << "][port:" << port << "][WSAError:" << WSAGetLastError() << "]";
		close_server();
		return false;
	}
	return true;
}

bool ServerTCPSocket::accept_connection(SOCKET* client_socket, int default_timeout) {
	*client_socket = accept(server_socket_, NULL, NULL);
	if (*client_socket == INVALID_SOCKET) {
		LOG_DEBUG << "Failed to call accept() " << "[listen_socket:" << server_socket_ << "][WSAError:" << WSAGetLastError() << "]";
		return false;
	}

	if (setsockopt(*client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*) &default_timeout, sizeof(default_timeout)) < 0) {
		LOG_DEBUG << "Failed to call setsockopt() " << "[listen_socket:" << server_socket_ << "][WSAError:" << WSAGetLastError() << "]";
		return false;
	}

	return true;
}

bool ServerTCPSocket::send_data(SOCKET client_socket, const char* data_send) {
	int retval = send(client_socket, data_send, strlen(data_send), 0);
	if (retval == SOCKET_ERROR) {
		LOG_DEBUG << "Failed to send response to server " << "[socket:" << client_socket << "][buffer:" << data_send << "][size:" << strlen(data_send) << "][flags:" << NULL << "]";
		return false;
	}
	LOG_INFO << "Data sent to client: " << data_send;
	return true;
}

bool ServerTCPSocket::receive_data(SOCKET client_socket, char* data_receive, int size) {
	int retval = recv(client_socket, data_receive, size, 0);

	if (retval == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAETIMEDOUT) {
			LOG_DEBUG << "Failed to receive data from client - TIMEOUT " << "[socket:" << client_socket << "][buffer:" << data_receive << "][size:" << size << "][flags:" << NULL << "]";
		} else {
			LOG_DEBUG << "Failed to receive data from client -  " << "[socket:" << client_socket << "][buffer:" << data_receive << "][size:" << size << "][flags:" << NULL << "]";
		}
		return false;
	}

	if (retval > 0) {
		data_receive[retval] = '\0';
		LOG_INFO << "Data received from client: " << data_receive;
	} else if (retval < 0) {
		LOG_DEBUG << "Failed to receive data from client " << "[socket:" << client_socket << "][recvbuf:" << data_receive << "][size:"
				  << size << "][flags:" << NULL << "][WSAError:" << WSAGetLastError() << "]";
	}

	return retval > 0;
}

void ServerTCPSocket::close_server() {
	if (server_socket_ != INVALID_SOCKET) {
		closesocket(server_socket_);
		server_socket_ = INVALID_SOCKET;
	}
	WSACleanup();
}
