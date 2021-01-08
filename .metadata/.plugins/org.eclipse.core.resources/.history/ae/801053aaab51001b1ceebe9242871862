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

#ifndef SRC_REQUEST_CODE_HPP_
#define SRC_REQUEST_CODE_HPP_

#include <string>

namespace server {

enum RequestCode {
	REQ_CONNECT = 0,
	REQ_DIAG,
	REQ_DISCONNECT,
	REQ_ECHO,
	REQ_INIT,
	REQ_RESTART,
	REQ_COMMAND,
	REQ_COMMAND_A,
	REQ_COMMAND_B,
	REQ_COMMAND_F,
	REQ_COLD_RESET,
	REQ_WARM_RESET,
	REQ_POWER_OFF_FIELD,
	REQ_POWER_ON_FIELD
};

/**
 * requestCodeToString - convert an enum value to the matching string description.
 * @param r the request code to be converted.
 * @return the matching string description.
 */
inline const std::string requestCodeToString(RequestCode r) {
	switch (r) {
	case REQ_CONNECT:
		return "REQ_CONNECT";
	case REQ_DIAG:
		return "REQ_DIAG";
	case REQ_DISCONNECT:
		return "REQ_DISCONNECT";
	case REQ_ECHO:
		return "REQ_ECHO";
	case REQ_INIT:
		return "REQ_INIT";
	case REQ_RESTART:
		return "REQ_RESTART";
	case REQ_COMMAND:
		return "REQ_COMMAND";
	default:
		return "[Unknown Request Code]";
	}
}

} /* namespace server */

#endif /* SRC_REQUEST_CODE_HPP_ */
