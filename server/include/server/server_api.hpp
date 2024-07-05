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

	~ServerAPI() {
		delete engine_;
	}

	/**
	 * initServer - initialize the server with the given configuration file.
	 * @param path configuration file used to initialize the server.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket initServer(std::string path);

	/*
	 * getVersion - returns a ResponsePacket containing the DLL Version in the "response" field.
	 * The "response" field contains the DLL Version.
	 * The "response" field will be formatted as a simple string
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket getVersion();

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
	ResponsePacket echoClient(int id_client, DWORD timeout);

	/**
	 * diagClient - return details about the client and its underlying layers.
	 * The details will be in the field "response" of the ResponsePacket structure.
	 * The details will be formatted as: TheReaderName|TheReaderStatus|TheReaderProtocolUsed
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the data or error codes and error descriptions.
	 */
	ResponsePacket diagClient(int id_client, DWORD timeout);

	/**
	 * sendCommand - return a ResponsePacket struct containing the target's response in the "response" field.
	 * @param id_client the client's id to send request to.
	 * @param command the command that will be send to the given target.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the target's response or error codes (value under 0) and error descriptions in case of error.
	 */
	ResponsePacket sendCommand(int id_client, std::string command, DWORD timeout);

	/**
	 * sendTypeA - send an APDU command over RF Type A.
	 * @param id_client the client's id to send request to.
	 * @param command the APDU command to be sent to the smartcard.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the smartcard's response or error codes and error descriptions in case of error.
	 */
	ResponsePacket sendTypeA(int id_client, std::string command, DWORD timeout);

	/**
	 * sendTypeB - send an APDU command over RF Type B.
	 * @param id_client the client's id to send request to.
	 * @param command the APDU command to be sent to the smartcard.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the smartcard's response or error codes and error descriptions in case of error.
	 */
	ResponsePacket sendTypeB(int id_client, std::string command, DWORD timeout);

	/**
	 * sendTypeF - send an APDU command over RF Type F.
	 * @param id_client the client's id to send request to.
	 * @param command the APDU command to be sent to the smartcard.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the smartcard's response or error codes and error descriptions in case of error.
	 */
	ResponsePacket sendTypeF(int id_client, std::string command, DWORD timeout);

	/**
	 * sendTypeABF - send an APDU command over All RF Types.
	 * @param id_client the client's id to send request to.
	 * @param command the APDU command to be sent to the smartcard.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the smartcard's response or error codes and error descriptions in case of error.
	 */
	ResponsePacket sendTypeAllTypes(int id_client, std::string command, DWORD timeout);

	/**
	 * restartTarget - restart the given target.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket restartTarget(int id_client, DWORD timeout);

	/**
	 * stopClient - stop the given client and all its underlying layers.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket stopClient(int id_client, DWORD timeout);

	/**
	 * coldReset - perform power off power on and return atr in the response structure.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the atr or the error codes (under 0) and error descriptions
	 */
	ResponsePacket coldReset(int id_client, DWORD timeout);

	/**
	 * coldReset - perform the reset without power switch and return atr in the response structure.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing either the atr or the error codes (under 0) and error descriptions
	 */
	ResponsePacket warmReset(int id_client, DWORD timeout);

	/**
	 * powerOFFField - set the field OFF.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
 	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket powerOFFField(int id_client, DWORD timeout);

	/**
	 * powerONField - set the field ON.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
 	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket powerONField(int id_client, DWORD timeout);

	/**
	 * pollTypeA - Perform the Anti-Collisison detection in Type A only.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
 	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket pollTypeA(int id_client, DWORD timeout);

	/**
	 * pollTypeB - Perform the Anti-Collisison detection in Type A only.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
 	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket pollTypeB(int id_client, DWORD timeout);

	/**
	 * pollTypeF - Perform the Anti-Collisison detection in Type F only.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
 	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket pollTypeF(int id_client, DWORD timeout);

	/**
	 * pollTypeF - Perform the Anti-Collisison detection in All RF Type.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
 	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket pollTypeAllTypes(int id_client, DWORD timeout);

	/**
	 * getNotifications - Get all notifications buffered in the client
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
 	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions and the notifications.
	 */
	ResponsePacket getNotifications(int id_client, DWORD timeout);

	/**
	 * clearNotifications - Clear all notifications buffered in the client
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket clearNotifications(int id_client, DWORD timeout);

	/**
	 * automaticInterfaceSwitching - Allow an switching between 2 interfaces, need to be used with IDENTIV_470x.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
 	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket automaticInterfaceSwitching(int id_client, DWORD timeout);

	/**
	 * deactivate_Interface - deactivate_Interface from the client.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
 	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket deactivate_Interface(int id_client, DWORD timeout);

	/**
	 * activate_Interface - activate_Interface from the client.
	 * @param id_client the client's id to send request to.
	 * @param timeout the waiting time of the execution of the request.
 	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket activate_Interface(int id_client, DWORD timeout);

	/**
	 * stopServer - stop the server and all its clients and their underlying layers.
	 * @return a ResponsePacket struct containing possible error codes (under 0) and error descriptions.
	 */
	ResponsePacket stopServer();
};

}  // namespace server

#endif /* SERVER_HPP_ */
