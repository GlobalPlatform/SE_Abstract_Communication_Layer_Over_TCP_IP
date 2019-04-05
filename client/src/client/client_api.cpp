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

#include <atomic>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "plog/include/plog/Log.h"
#include "plog/include/plog/Appenders/ColorConsoleAppender.h"

#include "client/client_api.h"

namespace client {

ResponsePacket ClientAPI::initClient(std::string path, FlyweightTerminalFactory available_terminals, FlyweightRequests available_requests) {
	return engine_->initClient(path, available_terminals, available_requests);
}

ResponsePacket ClientAPI::connectClient(const char* reader, const char* ip, const char* port) {
	return engine_->connectClient(reader, ip, port);
}

ResponsePacket ClientAPI::disconnectClient() {
	return engine_->disconnectClient();
}

ResponsePacket ClientAPI::loadAndListReaders() {
	return engine_->loadAndListReaders();
}

} /* namespace client */
