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

#include "client/requests/command.hpp"
#include "plog/include/plog/Log.h"

namespace client {

ResponsePacket Command::run(ITerminalLayer* terminal, ClientEngine* client_engine, char unsigned command[], unsigned long int command_length) {
	LOG_INFO << "Request \"command\" is being processed";
	//Add for debug
	/*
	if (command_length == 1){
	LOG_DEBUG << "Command : " << command[0];
	Sleep(10000);
	LOG_DEBUG << "End Sleep : " << command[1];
	ResponsePacket response = { .response = "119000" };
	return response;

	return terminal->sendCommand(command, command_length);
	}
	*/
	//End add for debug
	return terminal->sendCommand(command, command_length);
}

} /* namespace client */
