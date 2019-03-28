/*
 * power_on_field.h
 *
 *  Created on: 26 Mar 2019
 *      Author: Castel Christopher
 */

#ifndef INCLUDE_CLIENT_REQUESTS_POWER_ON_FIELD_H_
#define INCLUDE_CLIENT_REQUESTS_POWER_ON_FIELD_H_

#include "request.h"
#include "../../terminal/terminals/terminal.h"

namespace client {
class ClientEngine;

class PowerOnField : public IRequest {
public:
	PowerOnField() {}
	~PowerOnField() {}
	ResponsePacket run(ITerminalLayer* terminal, ClientEngine* client_engine, char unsigned command[], DWORD command_length);
};

} /* namespace client */

#endif /* INCLUDE_CLIENT_REQUESTS_POWER_ON_FIELD_H_ */
