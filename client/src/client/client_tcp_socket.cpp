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

#include "client/client_tcp_socket.hpp"
#include "plog/include/plog/Log.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

bool ClientTCPSocket::initClient(const char* ip, const char* port) {
	// initialises Winsock
	int retval = WSAStartup(MAKEWORD(2, 2), &wsaData_);
	if (retval != 0) {
		LOG_DEBUG << "Failed to call WSAStartup()";
		return false;
	}

	ZeroMemory(&hints_, sizeof(hints_));
	hints_.ai_family = AF_UNSPEC;
	hints_.ai_socktype = SOCK_STREAM;
	hints_.ai_protocol = IPPROTO_TCP;

	// resolves the server address and port
	retval = getaddrinfo(ip, port, &hints_, &result_);
	if (retval != 0) {
		WSACleanup();
		LOG_DEBUG << "Failed to call getaddrinfo() " << "[IP:" << ip << "][port:" << port << "]";
		return false;
	}
	return true;
}

bool ClientTCPSocket::connectClient() {
	int retval = 0;
	struct addrinfo* ptr;
	for (ptr = result_; ptr != NULL; ptr = ptr->ai_next) {
		client_socket_ = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		LOG_DEBUG << "Socket = " << client_socket_;
		if (client_socket_ == INVALID_SOCKET) {
			LOG_DEBUG << "Failed to call socket() " << "[ip:" << ip_ << "][port:" << port_ << "][WSAError:" << WSAGetLastError() << "]";
			return false;
		}

		// connects to server
		retval = connect(client_socket_, ptr->ai_addr, (int) ptr->ai_addrlen);
		if (retval == SOCKET_ERROR) {
			closesocket(client_socket_);
			client_socket_ = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result_);

	if (client_socket_ == INVALID_SOCKET) {
		WSACleanup();
		LOG_DEBUG << "Failed to connect: server unreachable" << "[ip:" << ip_ << "][port:" << port_ << "]";
		return false;
	}
	return true;
}

bool ClientTCPSocket::sendData(const char* data_send) {
	int retval = send(client_socket_, data_send, strlen(data_send), 0);
	if (retval == SOCKET_ERROR) {
		LOG_DEBUG << "Failed to send response to server " << "[socket:" << client_socket_ << "][buffer:" << data_send << "][size:" << strlen(data_send) << "][flags:" << NULL << "]";
		return false;
	}
	LOG_INFO << "Data sent to server: " << data_send;
	return true;
}

bool ClientTCPSocket::receiveData(char* data_receive, int size) {
	int retval = recv(client_socket_, data_receive, size, 0);
	if (retval > 0) {
		data_receive[retval] = '\0';
		LOG_INFO << "Data received from server: " << data_receive;
	} else if (retval < 0) {
		LOG_DEBUG << "Failed to receive data from server " << "[socket:" << client_socket_ << "][recvbuf:" << data_receive << "][size:"
				  << size << "][flags:" << NULL << "][WSAError:" << WSAGetLastError() << "]";
	}
	return retval > 0;
}

void ClientTCPSocket::closeClient() {
	if (client_socket_ != INVALID_SOCKET) {
		closesocket(client_socket_);
		client_socket_ = INVALID_SOCKET;
	}
	WSACleanup();
}
