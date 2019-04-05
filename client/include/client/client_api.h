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

#ifndef CLIENT_API_H_
#define CLIENT_API_H_

#include "client_engine.h"
#include "requests/flyweight_requests.h"
#include "../constants/response_packet.h"
#include "../terminal/flyweight_terminal_factory.h"

namespace client {

typedef void (__stdcall *Callback)(const char* text);

class ClientAPI {
private:
	ClientEngine* engine_;
protected:
public:
	ClientAPI(Callback notifyConnectionLost, Callback notifyRequestReceived, Callback notifyResponseSent) {
		this->engine_ = new ClientEngine(notifyConnectionLost, notifyRequestReceived, notifyResponseSent);
	}

	virtual ~ClientAPI() {
		delete engine_;
	}

	/**
	 * initClient - initialize the client with the given configuration file.
	 * @param config_path configuration file used to initialize the client.
	 * @param available_terminals the set of available terminal implementations.
	 * @param available_requests the set of available requests.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket initClient(std::string config_path, FlyweightTerminalFactory available_terminals, FlyweightRequests available_requests);

	/**
	 * loadAndListReaders - load and list available readers.
	 * The "response" field contains the id and name of each reader.
	 * The "response" field will be formatted this way: ReaderID|ReaderName|...|...
	 * @return a ResponsePacket struct containing either the list of readers or error codes (under 0) and error descriptions.
	 */
	ResponsePacket loadAndListReaders();

	/**
	 * connectClient - connect to the given reader and to the server.
	 * @param reader
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket connectClient(const char* reader, const char* ip, const char* port);

	/**
	 * disconnectClient - disconnect the client from the server and disconnect the terminal.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket disconnectClient();
};

} /* namespace client */

#endif /* CLIENT_API_H_ */
