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

class ServerTCPSocket {
private:
	SOCKET server_socket_;
	WSADATA wsaData_;
	const char* ip_;
	const char* port_;
	struct addrinfo* result_;
	struct addrinfo hints_;

public:
	ServerTCPSocket() {}
	virtual ~ServerTCPSocket() {}

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
	 * receiveData - wait for data on the given socket.
	 * @param client_client the client socket.
	 * @param data_receive the data to be received.
	 * @param size size of the data_received parameter.
	 * @return a boolean indicating whether an error occurred.
	 */
	bool receiveData(SOCKET client_socket, char* data_receive, int size);

	/**
	 * sendData - send data on the given client socket.
	 * @param client_socket the client socket to sent data on.
	 * @param data_send the data to be sent.
	 * @return a boolean indicating whether an error occurred.
	 */
	bool sendData(SOCKET client_socket, const char* data_send);

	/**
	 * closeServer - cleanup and close the server.
	 */
	void closeServer();
};

#endif /* INCLUDE_SERVER_SERVER_TCP_SOCKET_HPP_ */
