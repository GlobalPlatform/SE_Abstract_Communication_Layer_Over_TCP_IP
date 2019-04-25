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

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "constants/callback.hpp"
#include "constants/default_values.hpp"
#include "constants/request_code.hpp"
#include "server/client_data.hpp"
#include "server/server_engine.hpp"

namespace server {

class ServerAPI {
private:
	ServerEngine* engine_;
public:
	ServerAPI(Callback notifyConnectionAccepted) {
		this->engine_ = new ServerEngine(notifyConnectionAccepted);
	}

	virtual ~ServerAPI() {}

	/**
	 * initServer - initialize the server with the given configuration file.
	 * @param path configuration file used to initialize the server.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket initServer(std::string path);

	/**
	 * startServer - start the server. The server is now able to listen to accept connections and to process incoming requests.
	 * @param ip the ip to listen to.
	 * @param port the port to listen to.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket startServer(const char* ip, const char* port);

	/*
	 * listclients - returns a ResponsePacket containing data in the "response" field.
	 * The "response" field contains the number of connected clients, their id and their name.
	 * The "response" field will be formatted this way: ClientsNumber|ClientID|ClientName|...|...
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket listClients();

	/**
	 * echoClient - return a ResponsePacket used to check that the client is working without error.
	 * The ResponsePacket struct contains no data in the "response" field.
	 * No errorvalue under 0 in the ResponsePacket struct means the client is working.
	 * @param id_client the client's id to send request to.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket echoClient(int id_client, DWORD timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * diagClient - return details about the client and its underlying layers.
	 * The details will be in the field "response" of the ResponsePacket structure.
	 * The details will be formatted as: TheReaderName|TheReaderStatus|TheReaderProtocolUsed
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the data or error codes and error descriptions.
	 */
	ResponsePacket diagClient(int id_client, DWORD timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * sendCommand - return a ResponsePacket struct containing the target's response in the "response" field.
	 * @param id_client the client's id to send request to.
	 * @param command the command that will be send to the given target.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the target's response or error codes (value under 0) and error descriptions in case of error.
	 */
	ResponsePacket sendCommand(int id_client, std::string command, DWORD timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * sendTypeA - send an APDU command over RF Type A.
	 * @param id_client the client's id to send request to.
	 * @param command the APDU command to be sent to the smartcard.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the smartcard's response or error codes and error descriptions in case of error.
	 */
	virtual ResponsePacket sendTypeA(int id_client, std::string command, DWORD timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * sendTypeB - send an APDU command over RF Type B.
	 * @param id_client the client's id to send request to.
	 * @param command the APDU command to be sent to the smartcard.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the smartcard's response or error codes and error descriptions in case of error.
	 */
	ResponsePacket sendTypeB(int id_client, std::string command, DWORD timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * sendTypeF - send an APDU command over RF Type F.
	 * @param id_client the client's id to send request to.
	 * @param command the APDU command to be sent to the smartcard.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the smartcard's response or error codes and error descriptions in case of error.
	 */
	ResponsePacket sendTypeF(int id_client, std::string command, DWORD timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * restartTarget - restart the given target.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket restartTarget(int id_client, DWORD timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * stopClient - stop the given client and all its underlying layers.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket stopClient(int id_client, DWORD timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * coldReset - perform power off power on and return atr in the response structure.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the atr or the error codes (under 0) and error descriptions
	 */
	ResponsePacket coldReset(int id_client, DWORD timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * coldReset - perform the reset without power switch and return atr in the response structure.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the atr or the error codes (under 0) and error descriptions
	 */
	ResponsePacket warmReset(int id_client, DWORD timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * powerOFFField - set the field OFF.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
 	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket powerOFFField(int id_client, DWORD timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * powerONField - set the field ON.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
 	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket powerONField(int id_client, DWORD timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * stopServer - stop the server and all its clients and their underlying layers.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket stopServer();
};

}  // namespace server

#endif /* SERVER_HPP_ */
