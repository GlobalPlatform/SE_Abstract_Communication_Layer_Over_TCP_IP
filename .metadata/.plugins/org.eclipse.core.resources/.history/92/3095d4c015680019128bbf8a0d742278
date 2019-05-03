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

#ifndef INCLUDE_SERVER_SERVER_TCP_SOCKET_HPP_
#define INCLUDE_SERVER_SERVER_TCP_SOCKET_HPP_

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

namespace server {

class ServerTCPSocket {
private:
	SOCKET server_socket_;
	WSADATA wsaData_;
	const char* ip_;
	const char* port_;
	struct addrinfo* result_;
	struct addrinfo hints_;
public:
	ServerTCPSocket() = default;
	~ServerTCPSocket() = default;

	/**
	 * startServer - initialize the server, after this call the server will listen to the incoming connections on the given ip and port.
	 * @param ip the ip listen to.
	 * @param port the port to listen to.
	 * @return a boolean indicating whether an error occurred.
	 */
	bool startServer(const char* ip, const char* port);

	/**
	 * acceptConnect - accept the next connection and set the default timeout used for its network operations.
	 * @param client_socket the pointer of the socket of the future client.
	 * @param timeout the timeout that will be used as the default timeout for receive calls.
	 * @return a boolean indicating whether an error occurred.
	 */
	bool acceptConnection(SOCKET* client_socket, int timeout);

	/**
	 * sendPacket - send packet on the socket.
	 * @param packet the packet to be sent.
	 * @return a boolean indicating whether an error occurred.
	 */
	bool sendPacket(SOCKET socket, const char* packet);

	/**
	 * receivePacket - receivePacket packet on the socket.
	 * @param packet the packet to be received.
	 * @return a boolean indicating whether an error occurred.
	 */
	bool receivePacket(SOCKET socket, char* packet);

	/**
	 * closeServer - cleanup and close the server.
	 */
	void closeServer();
private:
	bool sendData(SOCKET socket, const char* data, int size);
};

} /* namespace server */

#endif /* INCLUDE_SERVER_SERVER_TCP_SOCKET_HPP_ */
