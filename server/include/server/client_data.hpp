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

#ifndef CLIENT_DATA_HPP_
#define CLIENT_DATA_HPP_

#define DEFAULT_NAME "no name"

#include <string>
#include <winsock2.h>

namespace server {

class ClientData {
private:
	int id_;
	SOCKET socket_ = INVALID_SOCKET;
	std::string name_ = DEFAULT_NAME;
protected:
public:
	ClientData() {}
	ClientData(SOCKET socket, int id, std::string name);
	virtual ~ClientData() {}

	/**
	 * getId - return the integer value of the field "id".
	 * @return the client's id.
	 */
	int getId();

	/**
	 * getName - return the string value of the field "name".
	 * @return the client's name.
	 */
	std::string getName();

	/**
	 * getSocket - return the socket between the client and the server.
	 * @return the client's socket.
	 */
	SOCKET getSocket();

	/**
	 * setId - set client's id.
	 * The given id must be unique and stay unique.
	 * @param id the id to be set.
	 */
	void setId(int id);

	/**
	 * setName - set the client's name.
	 * @param name the name to be set.
	 */
	void setName(std::string name);

	/**
	 * setSocket - set the client's socket.
	 * @param socket the socket to be set.
	 */
	void setSocket(SOCKET socket);
};

} /* namespace server */

#endif /* CLIENT_DATA_HPP_ */
