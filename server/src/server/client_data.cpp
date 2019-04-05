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

#include "server/client_data.h"

namespace server {

ClientData::ClientData(SOCKET socket, int id, std::string name) {
	this->socket_ = socket;
	this->id_ = id;
	this->name_ = name;
}

int ClientData::getId() {
	return id_;
}

std::string ClientData::getName() {
	return name_;
}

SOCKET ClientData::getSocket() {
	return socket_;
}

void ClientData::setId(int id) {
	this->id_ = id;
}

void ClientData::setName(std::string name) {
	this->name_ = name;
}

void ClientData::setSocket(SOCKET socket) {
	this->socket_ = socket;
}

} /* namespace server */
