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

#include "client/client_api.hpp"
#include "client/requests/cold_reset.hpp"
#include "client/requests/command.hpp"
#include "client/requests/diag.hpp"
#include "client/requests/disconnect.hpp"
#include "client/requests/disconnect_HW.hpp"
#include "client/requests/echo.hpp"
#include "client/requests/power_off_field.hpp"
#include "client/requests/power_on_field.hpp"
#include "client/requests/request.hpp"
#include "client/requests/request.hpp"
#include "client/requests/restart_target.hpp"
#include "client/requests/send_typeA.hpp"
#include "client/requests/send_typeB.hpp"
#include "client/requests/send_typeF.hpp"
#include "client/requests/warm_reset.hpp"
#include "client/requests/poll_type_A.hpp"
#include "client/requests/poll_type_B.hpp"
#include "client/requests/poll_type_F.hpp"
#include "client/requests/automatic_interface_switching.hpp"
#include "client/requests/reconnect_HW.hpp"
#include "constants/request_code.hpp"
#include "terminal/factories/example_factory_pcsc_contact.hpp"
#include "terminal/factories/example_factory_pcsc_contactless.hpp"
#include "terminal/factories/example_factory_pcsc_contactless_IDENTIV.hpp"
#include "terminal/flyweight_terminal_factory.hpp"
#include "terminal/terminals/example_pcsc_contact.hpp"
#include "dll/dll_client_api_wrapper.h"

#include <iostream>

using namespace client;

void setCallbackConnectionLost(client::Callback handler) {
	notifyConnectionLost = handler;
}

void setCallbackRequestsReceived(client::Callback handler) {
	notifyRequestReceived = handler;
}

void setCallbackResponseSent(client::Callback handler) {
	notifyResponseSent = handler;
}

client::ClientAPI* createClientAPI() {
	ClientAPI* client = new client::ClientAPI(notifyConnectionLost, notifyRequestReceived, notifyResponseSent);
	return client;
}

void disposeClientAPI(client::ClientAPI* client) {
	delete client;
	client = NULL;
}

void disconnectClient(client::ClientAPI* client, ResponseDLL& response_packet_dll) {
	ResponsePacket response_packet = client->disconnectClient();
	responsePacketForDll(response_packet, response_packet_dll);
}

void initClient(client::ClientAPI* client, const char* jsonConfig, ResponseDLL& response_packet_dll) {
	// config available terminal factories
	FlyweightTerminalFactory available_terminals;
	available_terminals.addFactory("EXAMPLE_PCSC_CONTACT", new ExamplePCSCContactFactory());
	available_terminals.addFactory("EXAMPLE_PCSC_CONTACTLESS", new ExamplePCSCContactlessFactory());
	available_terminals.addFactory("EXAMPLE_PCSC_CONTACTLESS_IDENTIV", new ExamplePCSCContactlessIDENTIVFactory());

	// config all requests the client can handle
	FlyweightRequests available_requests;
	available_requests.addRequest(REQ_COMMAND, new Command());
	available_requests.addRequest(REQ_COMMAND_A, new SendTypeA());
	available_requests.addRequest(REQ_COMMAND_B, new SendTypeB());
	available_requests.addRequest(REQ_COMMAND_F, new SendTypeF());
	available_requests.addRequest(REQ_DIAG, new Diag());
	available_requests.addRequest(REQ_DISCONNECT, new Disconnect());
	available_requests.addRequest(REQ_ECHO, new Echo());
	available_requests.addRequest(REQ_RESTART, new RestartTarget());
	available_requests.addRequest(REQ_COLD_RESET, new ColdReset());
	available_requests.addRequest(REQ_WARM_RESET, new WarmReset());
	available_requests.addRequest(REQ_POWER_OFF_FIELD, new PowerOffField());
	available_requests.addRequest(REQ_POWER_ON_FIELD, new PowerOnField());
	available_requests.addRequest(REQ_POLL_TYPE_A, new PollTypeA());
	available_requests.addRequest(REQ_POLL_TYPE_B, new PollTypeB());
	available_requests.addRequest(REQ_POLL_TYPE_F, new PollTypeF());
	available_requests.addRequest(REQ_AUTOMATIC_INTERFACE_SWITCHING, new AutomaticInterfaceSwitching());
	available_requests.addRequest(REQ_DISCONNECT_HW, new Disconnect_HW());
	available_requests.addRequest(REQ_RECONNECT_HW, new Reconnect_HW());

	ResponsePacket response_packet = client->initClient((jsonConfig != NULL) ? jsonConfig : "config/init.json", available_terminals, available_requests);
	responsePacketForDll(response_packet, response_packet_dll);
}

void loadAndListReaders(ClientAPI* client, ResponseDLL& response_packet_dll) {
	ResponsePacket response_packet = client->loadAndListReaders();
	responsePacketForDll(response_packet, response_packet_dll);
}

void connectClient(client::ClientAPI* client, const char* reader, const char* ip, const char* port, ResponseDLL& response_packet_dll) {
	ResponsePacket response_packet = client->connectClient(reader, ip, port);
	responsePacketForDll(response_packet, response_packet_dll);
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

void automaticInterfaceSwitching(client::ClientAPI* client, ResponseDLL& response_packet_dll){
	ResponsePacket response_packet = client->automaticInterfaceSwitching();
	responsePacketForDll(response_packet, response_packet_dll);
}

