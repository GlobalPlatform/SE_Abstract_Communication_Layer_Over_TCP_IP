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

#include "constants/request_code.hpp"
#include "dll/dll_server_api_wrapper.h"
#include "server/server_api.hpp"

#include <iostream>

using namespace server;

 server::ServerAPI* createServerAPI() {
	ServerAPI* server = new ServerAPI(notifyConnectionAccepted);
	return server;
}

 void setCallbackConnectionAccepted(Callback handler) {
	notifyConnectionAccepted = handler;
}

 void initServer(server::ServerAPI* server, const char* path, ResponseDLL& response_packet) {
	ResponsePacket response = server->initServer(path);
	responsePacketForDll(response, response_packet);
}

 void startServer(server::ServerAPI* server, const char* ip, const char* port, ResponseDLL& response_packet) {
	ResponsePacket response = server->startServer(ip, port);
	responsePacketForDll(response, response_packet);
}

 void listClients(server::ServerAPI* server, ResponseDLL& response_packet) {
	ResponsePacket response = server->listClients();
	responsePacketForDll(response, response_packet);
}

 void echoClient(server::ServerAPI* server, int id_client, ResponseDLL& response_packet) {
	ResponsePacket response = server->echoClient(id_client);
	responsePacketForDll(response, response_packet);
}

 void diagClient(server::ServerAPI* server, int id_client, ResponseDLL& response_packet) {
	ResponsePacket response = server->diagClient(id_client);
	responsePacketForDll(response, response_packet);
}

 void sendCommand(server::ServerAPI* server, int id_client, char* command, ResponseDLL& response_packet) {
	ResponsePacket response = server->sendCommand(id_client, command);
	responsePacketForDll(response, response_packet);
}

 void sendTypeA(server::ServerAPI* server, int id_client, char* command, ResponseDLL& response_packet) {
	ResponsePacket response = server->sendTypeA(id_client, command);
	responsePacketForDll(response, response_packet);
}

 void sendTypeB(server::ServerAPI* server, int id_client, char* command, ResponseDLL& response_packet) {
	ResponsePacket response = server->sendTypeB(id_client, command);
	responsePacketForDll(response, response_packet);
}

 void sendTypeF(server::ServerAPI* server, int id_client, char* command, ResponseDLL& response_packet) {
	ResponsePacket response = server->sendTypeF(id_client, command);
	responsePacketForDll(response, response_packet);
}

 void restartTarget(server::ServerAPI* server, int id_client, ResponseDLL& response_packet) {
	ResponsePacket response = server->restartTarget(id_client);
	responsePacketForDll(response, response_packet);
}

 void stopClient(server::ServerAPI* server, int id_client, ResponseDLL& response_packet) {
	ResponsePacket response = server->stopClient(id_client);
	responsePacketForDll(response, response_packet);
}

 void coldReset(server::ServerAPI* server, int id_client, ResponseDLL& response_packet) {
	ResponsePacket response = server->coldReset(id_client);
	responsePacketForDll(response, response_packet);
}

 void warmReset(server::ServerAPI* server, int id_client, ResponseDLL& response_packet) {
	ResponsePacket response = server->warmReset(id_client);
	responsePacketForDll(response, response_packet);
}

 void powerOFFField(server::ServerAPI* server, int id_client, ResponseDLL& response_packet) {
	ResponsePacket response = server->powerOFFField(id_client);
	responsePacketForDll(response, response_packet);
}

 void powerONField(server::ServerAPI* server, int id_client, ResponseDLL& response_packet) {
	ResponsePacket response = server->powerONField(id_client);
	responsePacketForDll(response, response_packet);
}

 void stopServer(server::ServerAPI* server, ResponseDLL& response_packet) {
	ResponsePacket response = server->stopServer();
	responsePacketForDll(response, response_packet);
}

 void disposeServerAPI(server::ServerAPI* server) {
	delete server;
}

void responsePacketForDll(ResponsePacket response_packet, ResponseDLL& response_packet_dll) {
	strcpy(response_packet_dll.response, response_packet.response.c_str());

	response_packet_dll.err_server_code = response_packet.err_server_code;
	strcpy(response_packet_dll.err_server_description, response_packet.err_server_description.c_str());

	response_packet_dll.err_client_code = response_packet.err_client_code;
	strcpy(response_packet_dll.err_client_description, response_packet.err_client_description.c_str());

	response_packet_dll.err_terminal_code = response_packet.err_terminal_code;
	strcpy(response_packet_dll.err_terminal_description, response_packet.err_terminal_description.c_str());

	response_packet_dll.err_card_code = response_packet.err_card_code;
	strcpy(response_packet_dll.err_card_description, response_packet.err_card_description.c_str());
}
