/*
 * cold_reset.h
 *
 *  Created on: 26 Mar 2019
 *      Author: Castel Christopher
 */

#ifndef INCLUDE_CLIENT_REQUESTS_COLD_RESET_H_
#define INCLUDE_CLIENT_REQUESTS_COLD_RESET_H_

#include "request.h"
#include "../../terminal/terminals/terminal.h"

namespace client {
class ClientEngine;

class ColdReset: public IRequest {
public:
	ColdReset() {}
	~ColdReset() {}
	ResponsePacket run(ITerminalLayer* terminal, ClientEngine* client_engine, char unsigned command[], DWORD command_length);
};

}

#endif /* INCLUDE_CLIENT_REQUESTS_COLD_RESET_H_ */
