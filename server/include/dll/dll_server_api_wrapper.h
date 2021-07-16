/*********************************************************************************
 Copyright 2020 GlobalPlatform, Inc.

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

#ifdef ADD_EXPORTS
#define ADDAPI __declspec(dllexport)
#else
#define ADDAPI __declspec(dllimport)
#endif

#define ADDCALL __cdecl

#include "server/server_api.hpp"
#include "constants/default_values.hpp"
#include "constants/callback.hpp"
#include "constants/response_packet.hpp"

struct ResponseDLL {
	char response[DEFAULT_DLL_BUFFER_SIZE_EXTENDED];
	long int err_server_code;
	char err_server_description[DEFAULT_DLL_BUFFER_SIZE];
	long int err_client_code;
	char err_client_description[DEFAULT_DLL_BUFFER_SIZE];
	long int err_terminal_code;
	char err_terminal_description[DEFAULT_DLL_BUFFER_SIZE];
	long int err_card_code;
	char err_card_description[DEFAULT_DLL_BUFFER_SIZE];
};

#ifdef __cplusplus
extern "C" {
#endif

server::Callback notifyConnectionAccepted = 0;

ADDAPI void setCallbackConnectionAccepted(server::Callback handler);

ADDAPI server::ServerAPI* createServerAPI();
ADDAPI void disposeServerAPI(server::ServerAPI* server);
void responsePacketForDll(server::ResponsePacket response_packet, ResponseDLL& response_packet_dll);

ADDAPI void initServer(server::ServerAPI* server, const char* jsonConfig, ResponseDLL& response_packet);
ADDAPI void getVersion(server::ServerAPI* server, ResponseDLL& response_packet);
ADDAPI void startServer(server::ServerAPI* server, const char* ip, const char* port, ResponseDLL& response_packet);
ADDAPI void stopServer(server::ServerAPI* server, ResponseDLL& response_packet);
ADDAPI void restartTarget(server::ServerAPI* server, int id_client, ResponseDLL& response_packet);

ADDAPI void listClients(server::ServerAPI* server, ResponseDLL& response_packet);
ADDAPI void echoClient(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void diagClient(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);

ADDAPI void sendCommand(server::ServerAPI* server, int id_client, char* command, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void sendTypeA(server::ServerAPI* server, int id_client, char* command, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void sendTypeB(server::ServerAPI* server, int id_client, char* command, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void sendTypeF(server::ServerAPI* server, int id_client, char* command, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void sendTypeAllTypes(server::ServerAPI* server, int id_client, char* command, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void stopClient(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void coldReset(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void warmReset(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void powerOFFField(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void powerONField(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void pollTypeA(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void pollTypeB(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void pollTypeF(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void pollTypeAllTypes(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void getNotifications(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);
ADDAPI void clearNotifications(server::ServerAPI* server, int id_client, DWORD timeout, ResponseDLL& response_packet);

#ifdef __cplusplus
}
#endif
