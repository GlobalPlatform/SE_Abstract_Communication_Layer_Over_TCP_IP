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

#ifdef ADD_EXPORTS
#define ADDAPI __declspec(dllexport)
#else
#define ADDAPI __declspec(dllimport)
#endif

#define ADDCALL __cdecl

#include "client/client_api.hpp"
#include "constants/callback.hpp"
#include "constants/response_packet.hpp"

struct ResponseDLL {
	char response[2048];

	long int err_server_code;
	char err_server_description[2048];

	long int err_client_code;
	char err_client_description[2048];

	long int err_terminal_code;
	char err_terminal_description[2048];

	long int err_card_code;
	char err_card_description[2048];
};

#ifdef __cplusplus
extern "C" {
#endif

client::Callback notifyConnectionLost = 0;
client::Callback notifyRequestReceived = 0;
client::Callback notifyResponseSent = 0;

ADDAPI void setCallbackConnectionLost(client::Callback handler);
ADDAPI void setCallbackRequestsReceived(client::Callback handler);
ADDAPI void setCallbackResponseSent(client::Callback handler);

ADDAPI client::ClientAPI* createClientAPI();
ADDAPI void connectClient(client::ClientAPI* client, const char* reader, const char* ip, const char* port, ResponseDLL& response_packet);
ADDAPI void disconnectClient(client::ClientAPI* client, ResponseDLL& response_packet_dll);
void responsePacketForDll(client::ResponsePacket response_packet, ResponseDLL& response_packet_dll);

ADDAPI void disposeClientAPI(client::ClientAPI* client);
ADDAPI void initClient(client::ClientAPI* client, ResponseDLL& response_packet);
ADDAPI void loadAndListReaders(client::ClientAPI*, ResponseDLL& response_packet);

#ifdef __cplusplus
}
#endif
