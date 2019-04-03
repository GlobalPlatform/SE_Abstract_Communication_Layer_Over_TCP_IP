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

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "nlohmann/json.hpp"
#include "plog/include/plog/Log.h"
#include "plog/include/plog/Appenders/ColorConsoleAppender.h"

#include "server/server_api.h"
#include "server/server_engine.h"
#include "constants/request_code.h"
#include "constants/response_packet.h"

namespace server {

ResponsePacket ServerAPI::initServer(std::string path) {
	ResponsePacket resp = engine->initServer(path);
	return resp;
}

ResponsePacket ServerAPI::startServer(const char* ip, const char* port) {
	return engine->startListening(ip, port);
}

ResponsePacket ServerAPI::listClients() {
	return engine->listClients();
}

ResponsePacket ServerAPI::sendCommand(int id_client, std::string command) {
	return engine->handleRequest(id_client, REQ_COMMAND, command);
}

ResponsePacket ServerAPI::sendTypeA(int id_client, std::string command) {
	return engine->handleRequest(id_client, REQ_COMMAND_A, command);
}

ResponsePacket ServerAPI::sendTypeB(int id_client, std::string command) {
	return engine->handleRequest(id_client, REQ_COMMAND_B, command);
}

ResponsePacket ServerAPI::sendTypeF(int id_client, std::string command) {
	return engine->handleRequest(id_client, REQ_COMMAND_F, command);
}

ResponsePacket ServerAPI::restartTarget(int id_client) {
	return engine->handleRequest(id_client, REQ_RESTART);
}

ResponsePacket ServerAPI::echoClient(int id_client) {
	return engine->handleRequest(id_client, REQ_ECHO);
}

ResponsePacket ServerAPI::diagClient(int id_client) {
	return engine->handleRequest(id_client, REQ_DIAG);
}

ResponsePacket ServerAPI::stopClient(int id_client) {
	ResponsePacket response = engine->handleRequest(id_client, REQ_DISCONNECT);
	if (response.err_server_code == ERR_NETWORK) {
		ResponsePacket okResponse;
		return okResponse;
	}
	return response;
}

ResponsePacket ServerAPI::coldReset(int id_client) {
	return engine->handleRequest(id_client, REQ_COLD_RESET);
}

ResponsePacket ServerAPI::warmReset(int id_client) {
	return engine->handleRequest(id_client, REQ_WARM_RESET);
}

ResponsePacket ServerAPI::powerOFFField(int id_client) {
	return engine->handleRequest(id_client, REQ_POWER_OFF_FIELD);
}

ResponsePacket ServerAPI::powerONField(int id_client) {
	return engine->handleRequest(id_client, REQ_POWER_ON_FIELD);
}

ResponsePacket ServerAPI::stopServer() {
	return engine->stopAllClients();
}

}  // namespace server

