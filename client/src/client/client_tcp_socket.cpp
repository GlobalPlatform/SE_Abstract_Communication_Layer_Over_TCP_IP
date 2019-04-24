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

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

namespace client {

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

bool ClientTCPSocket::sendData(const char* data, int size) {
	int retval = 0;
	do {
		retval = send(client_socket_, data, size, 0);
		if (retval == SOCKET_ERROR) {
			LOG_DEBUG << "Failed to send data to server - " << "[socket:" << client_socket_ << "][buffer:" << data << "][size:" << size << "][flags:" << NULL << "]";
			return false;
		}
		size -= retval;
		data += retval;
	} while (size > 0);

	return true;
}

bool ClientTCPSocket::sendPacket(const char* packet) {
	int packet_size = strlen(packet);
	int net_packet_size = htonl(packet_size); // deals with endianness

	// send packet's content size
	int retval = send(client_socket_, (char*) &net_packet_size, sizeof(int), 0);
	if (retval == SOCKET_ERROR || retval == 0) {
		LOG_DEBUG << "Failed to receive data from server -  " << "[socket:" << client_socket_ << "][buffer:" << net_packet_size << "][size:" << sizeof(int) << "][flags:" << NULL << "]";
		return false;
	}

	// send packet's content
	return sendData(packet, packet_size);
}

bool ClientTCPSocket::receivePacket(char* packet) {
	int received_size = 0;
	int net_received_size = 0;

	// retrieve packet size
	int retval = recv(client_socket_, (char*) &net_received_size, sizeof(int), MSG_WAITALL);
	if (retval == SOCKET_ERROR || retval == 0) {
		LOG_DEBUG << "Failed to receive data size from client -  " << "[socket:" << client_socket_ << "][buffer:" << received_size << "][size:" << sizeof(int) << "][flags:" << NULL << "]";
		return false;
	}
	received_size = ntohl(net_received_size); // deal with endianness

	// retrieve packet
	retval = recv(client_socket_, packet, received_size, MSG_WAITALL); // wait all received_size bytes to be received
	if (retval == SOCKET_ERROR || retval == 0) {
		LOG_DEBUG << "Failed to receive data size from client -  " << "[socket:" << client_socket_ << "][buffer:" << received_size << "][size:" << sizeof(int) << "][flags:" << NULL << "]";
		return false;
	}
	packet[retval] = '\0';
	return true;
}

void ClientTCPSocket::closeClient() {
	if (client_socket_ != INVALID_SOCKET) {
		closesocket(client_socket_);
		client_socket_ = INVALID_SOCKET;
	}
	WSACleanup();
}

} /* namespace client */
