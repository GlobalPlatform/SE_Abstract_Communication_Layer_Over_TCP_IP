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

#ifndef CLIENT_ENGINE_H_
#define CLIENT_ENGINE_H_

#include <atomic>
#include <thread>
#include <ws2tcpip.h>

#include "requests/flyweight_requests.h"
#include "../config/config_wrapper.h"
#include "../constants/response_packet.h"
#include "../terminal/flyweight_terminal_factory.h"
#include "../terminal/terminals/terminal.h"

namespace client {

typedef void (__stdcall *Callback)(const char* text);

class ClientEngine {
private:
	ConfigWrapper& config = ConfigWrapper::getInstance();
	SOCKET client_socket;
	std::thread requests_thread;
	std::atomic<bool> connected { false };
	std::atomic<bool> initialized { false };
	ITerminalLayer* terminal;
	FlyweightRequests requests;
	Callback notifyConnectionLost, notifyRequestReceived, notifyResponseSent;
public:
	ClientEngine(Callback notifyConnectionLost, Callback notifyRequestReceived, Callback notifyResponseSent) {
		this->notifyConnectionLost = notifyConnectionLost;
		this->notifyRequestReceived = notifyRequestReceived;
		this->notifyResponseSent = notifyResponseSent;
	}
	virtual ~ClientEngine();

	/**
	 * initClient - initialize the client with the given configuration file.
	 * @param config_path configuration file used to initialize the client.
	 * @param available_terminals the set of available terminal implementations.
	 * @param available_requests the set of available requests.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket initClient(std::string path, FlyweightTerminalFactory available_terminals, FlyweightRequests available_requests);

	/**
	 * loadAndListReaders - load and list available readers.
	 * The "response" field contains the id and name of each reader.
	 * The "response" field will be formatted this way: ReaderID|ReaderName|...|...
	 * @return a ResponsePacket struct containing either the list of readers or error codes (under 0) and error descriptions.
	 */
	ResponsePacket loadAndListReaders();

	/**
	 * connectClient - connect to the given reader and to the server.
	 * @param terminal_key the key corresponding to a reader.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket connectClient(const char* reader, const char* ip, const char* port);

	/**
	 * disconnectClient - disconnect the cliet from the server and disconnect the terminal.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket disconnectClient();

	/**
	 * waitingRequests - wait for requests on the given socket by using the helper function handleRequest.
	 * @return a ResponsePacket struct containing either the request's response or error codes (under 0) and error descriptions.
	 */
	ResponsePacket waitingRequests();

	/**
	 * handleRequest - helper function that performs async actions according to the given request.
	 * The response will be sent back to the given socket.
	 * @param socket the socket to which the response will be sent.
	 * @param request the request to be performed.
	 * @return a ResponsePacket struct containing either the request's response or error codes (under 0) and error descriptions.
	 */
	ResponsePacket handleRequest(SOCKET socket, std::string request);

	/**
	 * setStopFlag - set the flag used to stop (or not) waiting for requests.
	 * @param stop_flag the new value of the stop flag.
	 */
	void setConnectedFlag(bool stop_flag);
};

} /* namespace client */

#endif /* CLIENT_ENGINE_H_ */
