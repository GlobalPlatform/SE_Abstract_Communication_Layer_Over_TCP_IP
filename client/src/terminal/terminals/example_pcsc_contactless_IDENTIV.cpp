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

#include "constants/response_packet.hpp"
#include "terminal/terminals/example_pcsc_contactless_IDENTIV.hpp"
#include "terminal/terminals/utils/type_converter.hpp"
#include "plog/include/plog/Log.h"

#include <iostream>
#include <winscard.h>
#include <map>

#define TRIES_LIMIT 3
#if 1
#define TRANSP_CLASS 0xff
#define TRANSP_INS 0xc2
#define START_SESS_TAG 0x81
#define END_SESS_TAG 0x82
#define SWITCH_PROT_TAG 0x8f
#define FIELD_OFF_TAG 0x83
#define FIELD_ON_TAG 0x84

#define GET_STATUS_TAG 0xc0
#define ICC_RESP_TAG 0x97
#define TRANSCEIVE_TAG 0x95
#define TIMER_HI_TAG  0x5f
#define TIMER_LO_TAG  0x46
#endif


namespace client {

ExampleTerminalPCSCContactless_IDENTIV::~ExampleTerminalPCSCContactless_IDENTIV() {
	if (SCardFreeMemory(hContext_, mszReaders_) != SCARD_S_SUCCESS) {
		LOG_DEBUG << "Failed to call SCardFreeMemory() " << "[card:" << hCard_ << "][mszReaders:" << mszReaders_ << "]";
	}

	if (SCardReleaseContext(hContext_) != SCARD_S_SUCCESS) {
		LOG_DEBUG << "Failed to call SCardReleaseContext() " << "[card:" << hCard_ << "][hContext:" << hContext_ << "]";
	}
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::init() {
	ResponsePacket response;
	LONG resp;
	currentPollingType_ = TYPE_ABF;
	if ((resp = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext_)) != SCARD_S_SUCCESS) {
		LOG_DEBUG << "Failed to call SCardEstablishContext() [error:" << errorToString(resp) << "]"
				  << "[dwScope:" << SCARD_SCOPE_SYSTEM << "][pvReserved1:" << NULL << "][pvReserved1:" << NULL << "][hContext:" << hContext_ << "]";
		return handleErrorResponse("Failed to establish context", resp);
	}

	LOG_INFO << "Terminal PCSC initialized";
	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::loadAndListReaders() {
	ResponsePacket response;
	LONG resp;

	if (SCardFreeMemory(hContext_, mszReaders_) != SCARD_S_SUCCESS) {
		LOG_DEBUG << "Failed to call SCardFreeMemory() " << "[card:" << hCard_ << "][mszReaders:" << mszReaders_ << "]";
	}

	dwReaders_ = SCARD_AUTOALLOCATE;
	int tries = 0;
	if ((resp = SCardListReaders(hContext_, NULL, (LPTSTR) &mszReaders_, &dwReaders_)) != SCARD_S_SUCCESS) {
		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			resp = handleRetry();
			resp = SCardListReaders(hContext_, NULL, (LPTSTR) &mszReaders_, &dwReaders_);
			if (resp != SCARD_S_SUCCESS) {
				tries++;
			}
		}
		if (resp != SCARD_S_SUCCESS) {
			return handleErrorResponse("Failed to connect", resp);
		}
	}

	LPTSTR pReader = mszReaders_;
	std::string list_readers;
	int i = 0;
	while ('\0' != *pReader) {
		LOG_DEBUG << "Loaded reader: " << pReader;
		list_readers += std::to_string(i) + "|" + std::string((const char*) pReader) + "|";
		pReader = pReader + strlen((const char*) pReader) + 1;
		i++;
	}

	LOG_INFO << "Readers loaded";
	response.response = list_readers;
	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::connect(const char* reader) {
	ResponsePacket response;
	LONG resp;

	int tries = 0;
	if ((resp = SCardConnect(hContext_, reader, SCARD_SHARE_DIRECT, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard_, &dwActiveProtocol_)) != SCARD_S_SUCCESS) {
//	if ((resp = SCardConnect(hContext_, reader, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard_, &dwActiveProtocol_)) != SCARD_S_SUCCESS) {
		this->current_reader_ = std::string(reader);

		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			resp = handleRetry();
			tries++;
		}

		if (resp != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call SCardConnect() " << "[error:" << errorToString(resp) << "]"
					  << "[hContext:" << hContext_ << "][szReader:" << reader << "][dwShareMode:" << SCARD_SHARE_DIRECT<< "]"
					  << "[dwPreferredProtocols:" << 0 << "][hCard:" << hCard_ << "][dwActiveProtocol:" << dwActiveProtocol_ << "]";
			return handleErrorResponse("Failed to connect", resp);
		}
	}

	this->current_reader_ = std::string(reader);
	switch (dwActiveProtocol_) {
	case SCARD_PROTOCOL_T0:
		pioSendPci_ = *SCARD_PCI_T0;
		break;

	case SCARD_PROTOCOL_T1:
		pioSendPci_ = *SCARD_PCI_T1;
		break;
	}

	LOG_DEBUG << "Reader connected: " << reader << " Protocol Type : " << pioSendPci_.dwProtocol;
	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::sendCommand(unsigned char command[], DWORD command_length) {
	LONG resp = SCARD_SWALLOWED;
//	std::string strCommand = utils::unsignedCharToString(command, command_length);
	dwRecvLength_ = sizeof(pbRecvBuffer_);

	int tries = 0;
	if ((resp = SCardTransmit(hCard_, &pioSendPci_, command, command_length, NULL, pbRecvBuffer_, &dwRecvLength_)) != SCARD_S_SUCCESS) {
		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			resp = handleRetry();
			resp = SCardTransmit(hCard_, &pioSendPci_, command, command_length, NULL, pbRecvBuffer_, &dwRecvLength_);
			tries++;
		}
		if (resp != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call SCardTransmit() [error:" << errorToString(resp) << "]" << "[card:" << hCard_ << "][pbSendBuffer:" << command << "][cbSendLength:" << command_length << "]"
					  << "[recvbuffer:" << pbRecvBuffer_ << "][recvlength:" << dwRecvLength_ << "]";
			return handleErrorResponse("Failed to send command", resp);
		}
	}

	std::string responseAPDU =  utils::unsignedCharToString(pbRecvBuffer_, dwRecvLength_);
	ResponsePacket response = { .response = responseAPDU };
	return response;
}

int ExampleTerminalPCSCContactless_IDENTIV::sendEscapeCommand(unsigned char* command, unsigned long int* command_length) {
	LONG resp = SCARD_SWALLOWED;
	//std::string strCommand = utils::unsignedCharToString(command, command_length);
	unsigned long int lenReceived;
	unsigned long int IOCTL_CCID_ESCAPE3500 = SCARD_CTL_CODE(3500);


//	APDU[APDU_Len++] = 0x00;

	LOG_DEBUG << "CommandEscape Input, Command : " << utils::unsignedCharToString(command, *command_length) << ", Command_Length : " << *command_length;


	dwRecvLength_ = sizeof(pbRecvBuffer_);

	LOG_DEBUG << "hCard_ After SendEscapeCommand Connect " << "[hCard:" << hCard_ << "]";

	if ((resp = SCardControl(hCard_, IOCTL_CCID_ESCAPE3500, command, *command_length, pbRecvBuffer_, dwRecvLength_, &lenReceived)) !=SCARD_S_SUCCESS) {

		LOG_DEBUG << "Failed to call SCardControl() [error:" << errorToString(resp) << "]" << "[card:" << hCard_ << "][pbSendBuffer:" << command << "][cbSendLength:" << *command_length << "]"
				  << "[recvbuffer:" << pbRecvBuffer_ << "][recvlength:" << dwRecvLength_ << "][lenreceived:" << lenReceived << "]";
		return -1;
	}
	/*
	int tries = 0;
	if ((resp = SCardTransmit(hCard_, &pioSendPci_, APDU, APDU_Len, NULL, pbRecvBuffer_, &dwRecvLength_)) != SCARD_S_SUCCESS) {
		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			resp = handleRetry();

			resp = SCardTransmit(hCard_, &pioSendPci_, APDU, APDU_Len, NULL, pbRecvBuffer_, &dwRecvLength_);
			tries++;
		}
		if (resp != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call SCardTransmit() [error:" << errorToString(resp) << "]" << "[card:" << hCard_ << "][pbSendBuffer:" << APDU << "][cbSendLength:" << APDU_Len << "]"
					  << "[recvbuffer:" << pbRecvBuffer_ << "][recvlength:" << dwRecvLength_ << "]";
			return -1;
		}
	}
	*/
	LOG_DEBUG << "CommandEscape Success, APDU : " << utils::unsignedCharToString(command, *command_length) << " Len : " << *command_length << " [recvbuffer:" << utils::unsignedCharToString(pbRecvBuffer_, lenReceived) << "][recvlength:" << lenReceived << "]";

	*command_length = lenReceived;
	for (unsigned int i=0; i<*command_length; i++){
		command[i] = pbRecvBuffer_[i];
	}

	return 0;

}

int ExampleTerminalPCSCContactless_IDENTIV::sendInternalCommand(unsigned char* command, unsigned long int* command_length) {
	LONG resp = SCARD_SWALLOWED;

	LOG_DEBUG << "Internal Command Input, APDU : " << utils::unsignedCharToString(command, *command_length) << ", APDU_Length : " << *command_length;


	dwRecvLength_ = sizeof(pbRecvBuffer_);

	LOG_DEBUG << "hCard_ After SendEscapeCommand Connect " << "[hCard:" << hCard_ << "]";

	int tries = 0;
	if ((resp = SCardTransmit(hCard_, &pioSendPci_, command, *command_length, NULL, pbRecvBuffer_, &dwRecvLength_)) != SCARD_S_SUCCESS) {
		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			resp = handleRetry();
			resp = SCardTransmit(hCard_, &pioSendPci_, command, *command_length, NULL, pbRecvBuffer_, &dwRecvLength_);
			tries++;
		}
		if (resp != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call SCardTransmit() [error:" << errorToString(resp) << "]" << "[card:" << hCard_ << "][pbSendBuffer:" << command << "][cbSendLength:" << *command_length << "]"
					  << "[recvbuffer:" << pbRecvBuffer_ << "][recvlength:" << dwRecvLength_ << "]";
			return -1;
		}
	}
	LOG_DEBUG << "CommandEscape Success, APDU : " << utils::unsignedCharToString(command, *command_length) << " Len : " << *command_length << " [recvbuffer:" << utils::unsignedCharToString(pbRecvBuffer_, dwRecvLength_) << "][recvlength:" << dwRecvLength_ << "]";

	*command_length = dwRecvLength_;
	for (unsigned int i=0; i<*command_length; i++){
		command[i] = pbRecvBuffer_[i];
	}

	return 0;

}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::sendTypeA(unsigned char command[],  unsigned long int command_length) {

	if (currentPollingType_ != TYPE_A) {
		LOG_DEBUG << "Send Type A, the current state is not Type A, the current state is : " << currentPollingType_;
		ResponsePacket response = { .response = "KO", .err_client_code = ERR_OTHER, .err_client_description = "Current state is not type A." };
		return response;
	}

	// send requested command
	return sendCommand(command, command_length);
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::sendTypeB(unsigned char command[],  unsigned long int command_length) {
	if (currentPollingType_ != TYPE_B) {
		LOG_DEBUG << "Send Type B, the current state is not Type B, the current state is : " << currentPollingType_;
		ResponsePacket response = { .response = "KO", .err_client_code = ERR_OTHER, .err_client_description = "Current state is not type B." };
		return response;
	}

	// send requested command
	return sendCommand(command, command_length);
}

void ExampleTerminalPCSCContactless_IDENTIV::setTypeFTransparentSession()
{
	unsigned long int APDU_Len ;
	long res ;
	unsigned char APDU[270];
	std::string APDU_cmd ;

	// set transparent session on
	// FF C2 00 00 02 81 00 00
	APDU[0] = TRANSP_CLASS ;
	APDU[1] = TRANSP_INS ;
	APDU[2] = 0x00 ;
	APDU[3] = 0x00 ;
	APDU[4] = 0x02 ;

	APDU[5] = START_SESS_TAG ;
	APDU[6] = 0x00 ;
	APDU[7] = 0x00 ;  // termination character
	APDU_Len = 8 ;
	APDU_cmd = utils::unsignedCharToString(APDU, APDU_Len);

	res =	sendEscapeCommand(APDU, &APDU_Len);

	if (res == -1) {
	   ResponsePacket response = { .response = "KO", .err_client_code = ERR_OTHER, .err_client_description = "error reader" };

	}

// FF C2 00 02 04 8F 02 03 00
	// switch to Felica
	APDU[0] = TRANSP_CLASS ;
	APDU[1] = TRANSP_INS ;
	APDU[2] = 0x00 ;
	APDU[3] = 0x02 ; // P2 = 2 for switch protocol
	APDU[4] = 0x04 ;  //length
	APDU[5] = SWITCH_PROT_TAG ;
	APDU[6] = 0x02 ;
	APDU[7] = 0x03 ; // FELICA framing
	APDU[8] = 0x00 ;
	APDU[9] = 0x00 ; // LE

	APDU_Len = 10 ;
	APDU_cmd = utils::unsignedCharToString(APDU, APDU_Len);
	LOG_DEBUG << "SCardTransmit() [pbSendBuffer:" << APDU_cmd << "][cbSendLength:" << APDU_Len << "]";

	res = sendEscapeCommand(APDU, &APDU_Len);

	if (res == -1) {
	   LOG_DEBUG << "error reader switch to Felica";

	}


	}	


void ExampleTerminalPCSCContactless_IDENTIV::endTypeFTransparentSession()
{
	unsigned long int APDU_Len ;
	long res ;
	unsigned char APDU[270];
	std::string APDU_cmd ;
	// set transparent session off
	// FF C2 00 00 02 82 00 00
	APDU[0] = TRANSP_CLASS ;
	APDU[1] = TRANSP_INS ;
	APDU[2] = 0x00 ;
	APDU[3] = 0x00 ;
	APDU[4] = 0x02 ;

	APDU[5] = END_SESS_TAG ;
	APDU[6] = 0x00 ;
	APDU[7] = 0x00 ;  // LE
	APDU_Len = 8 ;
	APDU_cmd = utils::unsignedCharToString(APDU, APDU_Len);

	LOG_DEBUG << "ClearTransparent session :" << APDU_cmd << "][cbSendLength:" << APDU_Len << "]";
	res =	sendEscapeCommand(APDU, &APDU_Len);

		if (res == -1) {
			ResponsePacket response = { .response = "KO", .err_client_code = ERR_OTHER, .err_client_description = "error 1." };
			LOG_DEBUG << "error 1";

		}


	}


/* take the APDU returned by the reader and retrieve the answer form the card
 *  form is C0 03 00 90 00 97 xx aa bb cc dd......90 00
 *  if error has been encountered or time first status word is different
 *  xx length of useful data
 */
int TypeFCheckAnswer(unsigned char answer[], unsigned long int* answer_length )
{
	int val = 0 ;
// check if answer[0] = 0xc0 and return code is 9000
	if ((answer[0] == GET_STATUS_TAG) && (answer[3] == 0x90) && (answer[4] == 0x00 ) && (answer[5] == ICC_RESP_TAG))
	{
		// answer is valid
		// copy from 5+2 to answer_length-1
		// adjust APDU and APDU_Len APDU_len = APDU_Len -
		for (long unsigned int xx= 7; xx < *answer_length; xx++){
			answer[xx-7] = answer[xx] ;
		}
		*answer_length = *answer_length -7 ;
	}
	else
	{//C0 03 01 64 01 90 00
		if ((answer[0] == GET_STATUS_TAG) && (answer[3] == 0x64) && (answer[4] == 0x01)) {
		// in this case answer length is 0 
        *answer_length = 0 ; 
        LOG_DEBUG << " TypeFCheckAnswer TIMEOUT : -2";
		val = -2 ;
		}
		else // other type of answer
			val = -1 ;
	}
	return val ;
}
	


/**
* Calculates a CRC16 (CCITT) from the provided payload bytes.
* @param payloadBS the bytes for which the CRC is to be calculated.
* @return the CRC for the payload.
*/
int TypeFCRC(unsigned char *payload, long int len)
{
    int crc = 0; // initial value
    int polynomial = 0x1021; // 0001 0000 0010 0001  (0, 5, 12)

    for(int x=0;x<len;x++)
    {
    	unsigned char b = payload[x];
        for (int i = 0; i < 8; i++)
        {
            boolean bit = ((b >> (7 - i) & 1) == 1);
            boolean c15 = ((crc >> 15 & 1) == 1);
            crc <<= 1;
            if(c15 ^ bit)
                crc ^= polynomial;
        }
    }
    crc &= 0xffff;
    return crc;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::sendTypeF(unsigned char command[],  unsigned long int command_length) {
	if (currentPollingType_ != TYPE_F) {
		LOG_DEBUG << "Send Type F, the current state is not Type F, the current state is : " << currentPollingType_;
		ResponsePacket response = { .response = "KO", .err_client_code = ERR_OTHER, .err_client_description = "Current state is not type F." };
		return response;
	}

    dwRecvLength_ = sizeof(pbRecvBuffer_);


	unsigned long int APDU_Len = command_length;
	unsigned char APDU[TRANSP_CMD_MAX_SIZE];

	unsigned int ii ;
	std::string responseAPDU;
    long  res ;
    setTypeFTransparentSession() ;

	// FF C2 00 01 08 95 06 06 00 8E FC 02 00 00
	ii = 0 ;
	APDU[ii++] = TRANSP_CLASS;
    APDU[ii++] = TRANSP_INS;
	APDU[ii++] = 0x00;
    APDU[ii++] = 0x01;
	APDU[ii++] = command_length + 7 + 2 ;  // length remaining part
	APDU[ii++] = TRANSCEIVE_TAG ;
	APDU[ii++] = command_length ; // length

    for (unsigned int i = 0; i < command_length ; i++){
       			APDU[ii++] = command[i];
    }




	// add timer object
	APDU[ii++] = TIMER_HI_TAG ; // tag for timer
	APDU[ii++] = TIMER_LO_TAG ;
	APDU[ii++] = 0x04 ;  // length 
	APDU[ii++] = 0x00 ;   // fixed  microseconds
    APDU[ii++] = 0x02 ;
	APDU[ii++] = 0x00 ;
	APDU[ii++] = 0x00 ;  
	APDU[ii++] = 0x00 ;  // LE
	APDU_Len = command_length + 8 + 7;
	LOG_DEBUG << "APDU to be sent: "  << APDU << " & length is : " << APDU_Len;
 
// command 7 bytes header transparent + 1 byte (length) + max 255 bytes + 8 bytes (prologue) = 272 bytes
	responseAPDU =  utils::unsignedCharToString(APDU, APDU_Len);
	res = sendEscapeCommand(APDU, &APDU_Len);

		if (res ==  -1) {
			ResponsePacket response = { .response = "KO", .err_client_code = ERR_OTHER, .err_client_description = "Error F." };
			response.response = "Send Type F Failed, wrong answer from the reader";
			return response;
		}
		else {
			// refine check on received packet

// answer 4 bytes (error status) + 1 byte(tag answer) + 1 byte (length) + 255 max + 2 bytes (9000)	=> max 263 bytes
			res = TypeFCheckAnswer(APDU,&APDU_Len);
			if (res ==  -1) {
				ResponsePacket response = { .response = "KO", .err_client_code = ERR_OTHER, .err_client_description = "Error F." };
				response.response = "Send Type F Failed, wrong answer from the reader";
				return response;
			}
			if (res == -2) {
			    ResponsePacket response ;
				response.response = "" ; // empty answer
				return response ; // NO CRC to be added
		}
	int crc = TypeFCRC(APDU,APDU_Len-2);
	APDU[APDU_Len-2] = ((crc&0xFF00)>>8);
	APDU[APDU_Len-1] = (crc&0xFF);
	responseAPDU =  utils::unsignedCharToString(APDU, APDU_Len);
	ResponsePacket response1 ;

	response1.response = responseAPDU;

	return response1;

	}
}
ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::isAlive() {

	if (currentPollingType_ == TYPE_F) {
	   unsigned char command[] = { 0x06, 0x00, 0xFF,0xFF, 0x00, 0x00 };
	   return sendTypeF(command, sizeof(command));
	}
	else
	{
	  unsigned char command[] = { 0x00, 0x00, 0x00, 0x00 };
	  return sendCommand(command, sizeof(command));
	}
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::diag() {
	ResponsePacket response;
	LONG resp;
	CHAR szReader[200];
	DWORD cch = 200;
	BYTE bAttr[32];
	DWORD cByte = 32;
	DWORD dwState, dwProtocol;

	int tries = 0;
	if ((resp = SCardStatus(hCard_, szReader, &cch, &dwState, &dwProtocol, (LPBYTE) &bAttr, &cByte)) != SCARD_S_SUCCESS) {
		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			resp = handleRetry();
			resp = SCardStatus(hCard_, szReader, &cch, &dwState, &dwProtocol, (LPBYTE) &bAttr, &cByte);
			tries++;
		}
		if (resp != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call SCardStatus()  [error:" << errorToString(resp) << "]"
					  << "[card:" << hCard_ << "][szReader:" << szReader << "][cch:" << cch << "][dwState:" << dwState << "][dwProtocol:" << dwProtocol << "][bAttr:" << bAttr << "][cByte:" << cByte <<"]";
			return handleErrorResponse("Failed to retrieve card status", resp);
		}
	}

	std::string status;
	switch (dwState) {
	case SCARD_ABSENT:
		status = "Card absent";
		break;
	case SCARD_PRESENT:
		status = "Card present";
		break;
	case SCARD_SWALLOWED:
		status = "Card swallowed";
		break;
	case SCARD_POWERED:
		status = "Card has power";
		break;
	case SCARD_NEGOTIABLE:
		status = "Card reset and waiting PTS negotiation";
		break;
	case SCARD_SPECIFIC:
		status = "Card has specific communication protocols set";
		break;
	default:
		status = "Unknown or unexpected card state";
		break;
	}

	std::string protocol;
	switch (dwProtocol) {
	case SCARD_PROTOCOL_RAW:
		protocol = "The Raw Transfer protocol is in use";
		break;
	case SCARD_PROTOCOL_T0:
		protocol = "The ISO 7816/3 T=0 protocol is in use";
		break;
	case SCARD_PROTOCOL_T1:
		protocol = "The ISO 7816/3 T=1 protocol is in use";
		break;
	default:
		protocol = "Unknown or unexpected protocol in use";
	}

	std::string readers(szReader);
	response.response = "Readers: " + readers + " | Status: " + status + " | Protocol: " + protocol;
	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::disconnect() {
	ResponsePacket response;
	LONG resp;

	int tries = 0;
	if ((resp = SCardDisconnect(hCard_, SCARD_LEAVE_CARD)) != SCARD_S_SUCCESS) {
		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			resp = handleRetry();
			resp = SCardDisconnect(hCard_, SCARD_LEAVE_CARD);
			tries++;
		}
		if (resp != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call SCardDisconnect() "
					  << "[card:" << hCard_ << "][dwDisposition:" << SCARD_LEAVE_CARD << "]";
			return handleErrorResponse("Failed to disconnect", resp);
		}
	}

	LOG_INFO << "Terminal PCSC disconnected successfully";
	powerONField();
	pollTypeAllTypes();
	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::disconnect_HW() {
	LOG_INFO << "Disconnect_HW called";

	return disconnect();
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::deactivate_Interface() {
	LOG_INFO << "Deactivate_Interface called";

	return powerOFFField();
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::activate_Interface() {
	LOG_INFO << "Activate_Interface called";

	powerONField();
	pollTypeAllTypes();

	ResponsePacket response;
	return response;
}


ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::reconnect_HW() {
/*
	LONG resp;
	ResponsePacket response;

	int tries = 0;
	LOG_INFO << "Reconnect_HW called";
	if ((resp = SCardConnect(hContext_, current_reader_.c_str(), SCARD_SHARE_DIRECT, 0, &hCard_, &dwActiveProtocol_)) != SCARD_S_SUCCESS) {
		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			resp = handleRetry();
			LOG_INFO << "[Retry] SCardConnect called";
			resp = SCardConnect(hContext_, current_reader_.c_str(), SCARD_SHARE_DIRECT, 0, &hCard_, &dwActiveProtocol_);
			tries++;
		}
		if (resp != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call SCardConnect() " << "[error:" << errorToString(resp) << "]" << "[hContext:" << hContext_ << "][szReader:" << current_reader_ << "][dwShareMode:"
					  << SCARD_SHARE_DIRECT << "]" << "[dwPreferredProtocols:" << 0 << "][hCard:" << hCard_ << "][dwActiveProtocol:" << dwActiveProtocol_ << "]";
			return handleErrorResponse("Failed to connect", resp);
		}
	}

	switch (dwActiveProtocol_) {
	case SCARD_PROTOCOL_T0:
		pioSendPci_ = *SCARD_PCI_T0;
		break;

	case SCARD_PROTOCOL_T1:
		pioSendPci_ = *SCARD_PCI_T1;
		break;
	}

	return response;
*/

	ResponsePacket response;
	LONG resp;

	int tries = 0;
	LOG_INFO << "Reconnect_HW called";
	if ((resp = SCardReconnect(hCard_, SCARD_SHARE_DIRECT, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_UNPOWER_CARD, &dwActiveProtocol_)) != SCARD_S_SUCCESS) {
		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			resp = handleRetry();
			LOG_INFO << "[Retry] SCardReconnect called";
			resp = SCardReconnect(hCard_, SCARD_SHARE_DIRECT, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_UNPOWER_CARD, &dwActiveProtocol_);
			Sleep(300);
			tries++;
		}
		if (resp != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call Reconnect_HW() "
					  << "[hCard_:" << hCard_ << "][[dwShareMode:" << SCARD_SHARE_DIRECT << "]"
					  << "[dwActiveProtocol:" << dwActiveProtocol_ << "]";
			return handleErrorResponse("Failed to cold reset", resp);
		}
	}

	switch (dwActiveProtocol_) {
	case SCARD_PROTOCOL_T0:
		pioSendPci_ = *SCARD_PCI_T0;
		break;

	case SCARD_PROTOCOL_T1:
		pioSendPci_ = *SCARD_PCI_T1;
		break;
	}

	return response;

}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::restart() {
	ResponsePacket response;
	LONG resp;
	DWORD dwProtocol;

	int tries = 0;
	LOG_INFO << "SCardReconnect called";
	if ((resp = SCardReconnect(hCard_, SCARD_SHARE_DIRECT, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_LEAVE_CARD, &dwProtocol)) != SCARD_S_SUCCESS) {
		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			resp = handleRetry();
			LOG_INFO << "[Retry] SCardReconnect called";
			resp = SCardReconnect(hCard_, SCARD_SHARE_DIRECT, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_LEAVE_CARD, &dwProtocol);
			tries++;
		}
		if (resp != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call SCardReconnect() "
					  << "[hContext:" << hContext_ << "][[dwShareMode:" << SCARD_SHARE_DIRECT << "]"
					  << "[dwActiveProtocol:" << dwActiveProtocol_ << "]";
			return handleErrorResponse("Failed to reconnect", resp);
		}
	}

	switch (dwActiveProtocol_) {
	case SCARD_PROTOCOL_T0:
		pioSendPci_ = *SCARD_PCI_T0;
		break;
	case SCARD_PROTOCOL_T1:
		pioSendPci_ = *SCARD_PCI_T1;
		break;
	}

	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::retrieveAtr(BYTE* bAttr, DWORD* cByte) {
	LONG resp;
	ResponsePacket response;
	DWORD cch = 200;
	DWORD dwState;

	int tries = 0;
	if ((resp = SCardStatus(hCard_, mszReaders_, &cch, &dwState, &dwActiveProtocol_, (LPBYTE) bAttr, cByte)) != SCARD_S_SUCCESS) {
		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			resp = handleRetry();
			resp = SCardStatus(hCard_, mszReaders_, &cch, &dwState, &dwActiveProtocol_, (LPBYTE) bAttr, cByte);
			tries++;
		}
		if (resp != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call SCardStatus() "
					  << "[hContext:" << hContext_ << "][[mszReaders:" << mszReaders_ << "]"
					  << "[dwActiveProtocol:" << dwActiveProtocol_ << "]";
			return handleErrorResponse("Failed to retrieve atr", resp);
		}
	}

	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::powerOFFField() {
	ResponsePacket response;
	int ret;

	LOG_DEBUG << "Contactless_IDENTIV Start PowerOFFField";
	//Get Power Field State
	if (currentPollingType_ == TYPE_F) {
			   endTypeFTransparentSession() ;
	}


	ret = powerFieldState();
	if (ret == -1) {
		response.response = "Get Power OFF Field: failed";
		return response;
	}

	LOG_DEBUG << "Set power OFF Field: ";
	// get initial polling
	unsigned char command[] = { 0x96,  0x00};
	DWORD commandLen = sizeof command;

	if (sendEscapeCommand(command, &commandLen) != 0x00) {
		LOG_DEBUG << "Set Power OFF Field: failed";
		response.response = "Set Power OFF Field: failed";
		return response;
	}

	if (commandLen < 0){
		LOG_DEBUG << "Set Power OFF Field failed: " << "Answer : " << command << " & length is : " << commandLen;
		response.response = "Failed to Set Power OFF Field, wrong answer from the reader";
		return response;

	}

	ret = powerFieldState();
	if (ret == -1) {
		response.response = "Get Power OFF Field: failed";
		return response;
	}

	LOG_DEBUG << "Set Power OFF Field: Success";

	return response;
}

int ExampleTerminalPCSCContactless_IDENTIV::reset_Field() {
	int ret;

	LOG_DEBUG << "Contactless_IDENTIV Start PowerOFFField";
	//Get Power Field State
	if (currentPollingType_ == TYPE_F) {
			   endTypeFTransparentSession() ;
    }
	ret = powerFieldState();
	if (ret < 0) {
		return ret;
	}

	LOG_DEBUG << "Set power OFF Field: ";
	// get initial polling
	unsigned char command[270];
	DWORD commandLen;

	command[0x00] = 0x96;
	command[0x01] = 0x00;
	commandLen = 0x02;

	ret = sendEscapeCommand(command, &commandLen);
	if (ret != 0x00) {
		LOG_DEBUG << "Set Power OFF Field: failed";
		return ERR_CMD_SET_FIELD_STATE;
	}

	if (commandLen != 0){
		LOG_DEBUG << "Set Power OFF Field failed: " << "Answer : " << command << " & length is : " << commandLen;
		return ERR_RET_SET_FIELD_STATE;

	}

	ret = powerFieldState();
	if (ret < 0) {
		return ret;
	}

	LOG_DEBUG << "Set Power OFF Field: Success";

	Sleep(250);

	command[0x00] = 0x96;
	command[0x01] = 0x01;
	commandLen = 2;

	ret = sendEscapeCommand(command, &commandLen);
	if (ret < 0x00) {
		LOG_DEBUG << "Set Power ON Field: failed";
		return ERR_CMD_SET_FIELD_STATE;
	}

	if (commandLen != 0){
		LOG_DEBUG << "Set Power ON Field failed: " << "Answer : " << command << " & length is : " << commandLen;
		return ERR_RET_SET_FIELD_STATE;

	}

	LOG_DEBUG << "Set Power ON Field: Success";

	ret = powerFieldState();
	if (ret < 0) {
		LOG_DEBUG << "Power ON Field State: failed";
		return ret;
	}
/*
	LONG resp;


	if ((resp = SCardReconnect(hCard_, SCARD_SHARE_DIRECT, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_RESET_CARD, &dwActiveProtocol_)) != SCARD_S_SUCCESS) {
		LOG_DEBUG << "Error in SCardReconnect";
		return ERR_CARD_RECONNECT;
		}
*/
	return RET_OK;
}
ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::powerOFF() {
	ResponsePacket response;
	std::string APDU_cmd ;
	unsigned long int APDU_Len ;
	long res ;

		unsigned char APDU[270];
	if (currentPollingType_ == TYPE_F) {
		setTypeFTransparentSession() ;

//RF OFF = FF C2 00 00 04 83 00 84 00 00
	// toggle field
	APDU[0] = TRANSP_CLASS ;
	APDU[1] = TRANSP_INS ;
	APDU[2] = 0x00 ;
	APDU[3] = 0x00 ;

	APDU[4] = 0x2  ;

	APDU[5] = FIELD_OFF_TAG;
	APDU[6] = 0x00 ;
	APDU[7] = 0x00 ;


    APDU_Len = 8 ;
    LOG_DEBUG << "Power OFF F " ;
    APDU_cmd = utils::unsignedCharToString(APDU, APDU_Len);
    LOG_DEBUG << "SCardTransmit() [pbSendBuffer:" << APDU_cmd << "][cbSendLength:" << APDU_Len << "]";


    res = sendEscapeCommand(APDU, &APDU_Len);

    		if (res == -1) {
    			LOG_DEBUG << "error 4";
    		}

	}
    else
    response =  powerOFFField() ;

return response ;
}



ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::powerONField() {
	ResponsePacket response;

	int ret;

	if (currentPollingType_ == TYPE_F) {
	    endTypeFTransparentSession() ;
	}

	ret = powerFieldState();
	if (ret == -1) {
		response.response = "Get Power OFF Field: failed";
		return response;
	}

	LOG_DEBUG << "Set power ON Field: ";
	unsigned char command[] = { 0x96,  0x01};
	DWORD commandLen = sizeof command;

	if (sendEscapeCommand(command, &commandLen) != 0x00) {
		LOG_DEBUG << "Set Power ON Field: failed";
		response.response = "Set Power ON Field: failed";
		return response;
	}

	if (commandLen != 0){
		LOG_DEBUG << "Set Power ON Field failed: " << "Answer : " << command << " & length is : " << commandLen;
		response.response = "Failed to Set Power ON Field, wrong answer from the reader";
		return response;

	}

	Sleep(500); // hardware needs delay
	// reconnect shared & T0|T1 protocol

	ret = powerFieldState();
	if (ret == -1) {
		response.response = "Get Power OFF Field: failed";
		return response;
	}

	return response;
}
ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::powerON() {
	ResponsePacket response;
	unsigned long int APDU_Len ;
	long res ;

		unsigned char APDU[270];
		std::string APDU_cmd ;

	if (currentPollingType_ == TYPE_F) {
		setTypeFTransparentSession() ;

	APDU[0] = TRANSP_CLASS ;
	APDU[1] = TRANSP_INS ;
	APDU[2] = 0x00 ;
	APDU[3] = 0x00 ;

	APDU[4] = 0x2  ;

	APDU[5] = FIELD_ON_TAG;
	APDU[6] = 0x00 ;  // termination byte
	APDU[7] = 0x00 ;

    APDU_Len = 8 ;
    LOG_DEBUG << "Power ON F " ;
    APDU_cmd = utils::unsignedCharToString(APDU, APDU_Len);
    	LOG_DEBUG << "SCardTransmit() [pbSendBuffer:" << APDU_cmd << "][cbSendLength:" << APDU_Len << "]";


    	res = sendEscapeCommand(APDU, &APDU_Len);

    		if (res == -1) {
    			LOG_DEBUG << "error 5";
    		}
    		Sleep(500); // hardware needs delay


	}
	else
		response = powerONField() ;
	return response ;
}

int ExampleTerminalPCSCContactless_IDENTIV::powerFieldState(){

	LOG_DEBUG << "Get power Field State: ";
	// get initial polling
	unsigned char command[270];
	DWORD commandLen = 0;

	command[commandLen++] = 0x96;
	command[commandLen++] = 0xFF;

	if (sendEscapeCommand(command, &commandLen) != 0x00) {
		LOG_DEBUG << "Get Power Field State: failed";
		return ERR_CMD_GET_FIELD_STATE;
	}

	if (commandLen != 1){
		LOG_DEBUG << "Get Power Field State failed: " << "Answer : " << command << " & length is : " << commandLen;
		return ERR_CMD_GET_FIELD_STATE;

	}

	switch (command[0x00]) {
	case 0x00:
		LOG_DEBUG << "Get Power Field State: Field ON";
		break;
	case 0x01:
		LOG_DEBUG << "Get Power Field State: Field OFF";
		break;
	default:
		LOG_DEBUG << "Get Power Field State: Unknown Field State";
		break;
	}

	return command[0x00];
}

int ExampleTerminalPCSCContactless_IDENTIV::getType(){

	ResponsePacket response;
	LOG_DEBUG << "Get Type: ";
	// get initial polling
	unsigned char command[270];
	DWORD commandLen;
	command[0x00] = 0x94;
	command[0x01] = 0xFF;
	commandLen = 0x02;

	if (sendEscapeCommand(command, &commandLen) != 0x00) {
		LOG_DEBUG << "Get Type: failed";
		response.response = "Not supported";
		return -1;
	}

	if (commandLen != 2){
		LOG_DEBUG << "Get Type failed: " << "Answer : " << command << " & length is : " << commandLen;
		response.response = "Failed to Get Type State, wrong answer from the reader";
		return -1;

	}

	LOG_DEBUG << "Get Type: ," << command;

	return 0x00;
}


ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::pollTypeA() {
	ResponsePacket response;

	LOG_DEBUG << "Set Polling Type A";

	int ret;

	ret = switchPollingType(TYPE_A);
	if (ret < 0){
		response.response = "Failed to set polling type A";
		return response;
	}

	LOG_DEBUG << "Set Polling Type A, Success";

	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::pollTypeB() {
	ResponsePacket response;

	LOG_DEBUG << "Set Polling Type B";

	int ret;

	ret = switchPollingType(TYPE_B);
	if (ret < 0){
		response.response = "Failed to set polling type B";
		return response;
	}

	LOG_DEBUG << "Set Polling Type B, Success";

	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::pollTypeF() {
	ResponsePacket response;
	DWORD commandLen = 0x00;

	int ret;
	LOG_DEBUG << "Set Polling Type F";

	ret = switchPollingType(TYPE_F);

	if (ret < 0) {
		response.response = "Error when switch Polling Type";
		return response;
	}
	LOG_DEBUG << "Send Polling Type F";

	unsigned char APDU[270];
	int res;
	std::string responseAPDU;

	   commandLen = 0x00;
	   // FF C2 00 01 08 95 06 06 00 8E FC 02 00 00
	   // FF C2 00 01 08 95 06 06 00 FF FF 02 00 00
		APDU[commandLen++] = TRANSP_CLASS;
		APDU[commandLen++] = TRANSP_INS;
		APDU[commandLen++] = 0x00;
		APDU[commandLen++] = 0x01;
		APDU[commandLen++] = 0x0F;

		APDU[commandLen++] = TRANSCEIVE_TAG;
		APDU[commandLen++] = 0x06;
		APDU[commandLen++] = 0x06;
		APDU[commandLen++] = 0x00;
		APDU[commandLen++] = 0xFF;
		APDU[commandLen++] = 0xFF;
		APDU[commandLen++] = 0x00;
		APDU[commandLen++] = 0x00;
		APDU[commandLen++] = TIMER_HI_TAG ;
		APDU[commandLen++] = TIMER_LO_TAG ;
		APDU[commandLen++] = 0x04 ;  // length

		APDU[commandLen++]  = 0x00;   // fixed   microseconds
		APDU[commandLen++] = 0x02 ;
		APDU[commandLen++]  = 0x00 ;
		APDU[commandLen++]  = 0x00 ;
		APDU[commandLen++] = 0x00 ;   // LE

		LOG_DEBUG << " apdu: " <<   APDU << " & length is : " << commandLen;

		res = sendEscapeCommand(APDU, &commandLen);

		if (res == -1) {
			 response.response = " Send Polling Type F: failed, wrong answer from the reader";
			 return response;
		}
		// C0 03 01 64 01 90 00
		if ((commandLen < 7 )| (APDU[3] == 0x64) |(APDU[commandLen - 2]!=0x90) | (APDU[commandLen - 1]!=0x00) ){
				 LOG_DEBUG << " Send Polling Type F: " << "Answer : " << APDU << " & length is : " << commandLen;
				 response.response = "";
				 return response;
			}



	LOG_DEBUG << "Send Polling Type F: success";

	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::pollTypeAllTypes() {
	ResponsePacket response;

	LOG_DEBUG << "Set Polling Type ABF";

	int ret;

	ret = switchPollingType(TYPE_ABF);
	if (ret < 0){
		response.response = "Failed to set polling type ABF";
		return response;
	}

	LOG_DEBUG << "Set Polling Type ABF, Success";

	return response;
}

int ExampleTerminalPCSCContactless_IDENTIV::switchPollingType(byte pollingType) {

	int ret = 0;
	unsigned char command[270];
	DWORD commandLen = 0x00;

	LOG_DEBUG << "Set Initial Polling: ";

    if (currentPollingType_ == TYPE_F) {
	    endTypeFTransparentSession() ;
	}

	// get initial polling

	command[commandLen++] = 0x95;
	command[commandLen++] = 0xFF;
	command[commandLen++] = pollingType;
	command[commandLen++] = 0x00;

	if (sendEscapeCommand(command, &commandLen) != 0x00) {
		LOG_DEBUG << "Set Initial Polling: failed";
		return ERR_CMD_GET_TYPE;
	}

	if (commandLen != 0){
		LOG_DEBUG << "Set Initial Polling failed, " << "Answer : " << command << " & length is : " << commandLen;
		return ERR_RET_GET_TYPE;

	}
	currentPollingType_ = pollingType;
	LOG_DEBUG << "Set Initial Polling (success)";

	ret = reset_Field();

	getType();


    if (pollingType == TYPE_F){
	// forcetypeF with transparent session
	    setTypeFTransparentSession() ;
  }



	return ret;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::getNotifications() {
	ResponsePacket response;
	response.response = "Not supported";
	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::clearNotifications() {
	ResponsePacket response;
	response.response = "Not supported";
	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::automaticInterfaceSwitching(){
	ResponsePacket response;

	LOG_DEBUG << "Automatic Interface Switching: ";

	// get initial polling
	unsigned char command[256];
	int i = 0;
	DWORD commandLen;

	i = 0;
	command[i++] = 0xB2;
	command[i++] = 0x00;
	commandLen = i;

	if (sendEscapeCommand(command, &commandLen) != 0x00) {
		LOG_DEBUG << "READER_LED_CONTROL_BY_FW: failed";
		response.response = "Not supported";
		return response;
	}

	if ((commandLen < 2) | (command[0x00]!=0x90) | (command[0x01]!=0x00) ){
		LOG_DEBUG << "READER_LED_CONTROL_BY_FW, " << "Answer : " << command << " & length is : " << commandLen;
		response.response = "Failed to READER_LED_CONTROL_BY_FW, wrong answer from the reader";
		return response;

	}

	i = 0;
	command[i++] = 0x19;
	command[i++] = 0x00;
	command[i++] = 0x01;
	commandLen = i;

	if (sendEscapeCommand(command, &commandLen) != 0x00) {
		LOG_DEBUG << "CNTLESS_SWITCH_RF_ON_OFF (RED): failed";
		response.response = "Not supported";
		return response;
	}

	if ((commandLen < 2) | (command[0x00]!=0x90) | (command[0x01]!=0x00) ){
		LOG_DEBUG << "CNTLESS_SWITCH_RF_ON_OFF (RED), " << "Answer : " << command << " & length is : " << commandLen;
		response.response = "Failed to CNTLESS_SWITCH_RF_ON_OFF (RED), wrong answer from the reader";
		return response;

	}


	i = 0;
	command[i++] = 0x19;
	command[i++] = 0x00;
	command[i++] = 0x01;
	commandLen = i;

	if (sendEscapeCommand(command, &commandLen) != 0x00) {
		LOG_DEBUG << "CNTLESS_SWITCH_RF_ON_OFF (GREEN): failed";
		response.response = "Not supported";
		return response;
	}

	if ((commandLen < 2) | (command[0x00]!=0x90) | (command[0x01]!=0x00) ){
		LOG_DEBUG << "CNTLESS_SWITCH_RF_ON_OFF (GREEN), " << "Answer : " << command << " & length is : " << commandLen;
		response.response = "Failed to CNTLESS_SWITCH_RF_ON_OFF (GREEN), wrong answer from the reader";
		return response;

	}

	i = 0;
	command[i++] = 0x9C;
	command[i++] = 0x01;
	commandLen = i;

	if (sendEscapeCommand(command, &commandLen) != 0x00) {
		LOG_DEBUG << "CNTLESS_SWITCH_RF_ON_OFF (Leave RF Field ON when contact card is present in the reader): failed";
		response.response = "Not supported";
		return response;
	}

	if ((commandLen < 2) | (command[0x00]!=0x90) | (command[0x01]!=0x00) ){
		LOG_DEBUG << "CNTLESS_SWITCH_RF_ON_OFF (Leave RF Field ON when contact card is present in the reader), " << "Answer : " << command << " & length is : " << commandLen;
		response.response = "Failed to CNTLESS_SWITCH_RF_ON_OFF (Leave RF Field ON when contact card is present in the reader), wrong answer from the reader";
		return response;

	}

/*
	i.	RESET On CL interface

	ii.	READER_LED_CONTROL_BY_FW
	FF CC 00 00 02 B2 00

	iii.	CNTLESS_SWITCH_RF_ON_OFF - 0-RED
	FF CC 00 00 03 19 00 01

	iv.	CNTLESS_SWITCH_RF_ON_OFF - 1-GREEN
	FF CC 00 00 03 19 01 01

	v.	CNTLESS_SWITCH_RF_ON_OFF - Leave RF Field ON when contact card is present in the reader
	FF CC 00 00 02 9C 01

	vi.	Keep the card in Contact interface and open another dedicated client for the interface
	Perform a RESET
*/

	return response;
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::coldReset() {
	ResponsePacket response;
	LONG resp;
	DWORD dwProtocol;
	std::string s1 ;

	int tries = 0;
	if (currentPollingType_ == TYPE_F) {

		// keep transparent session
		// do field off/on
		   powerOFF() ;
		   powerON();

		   	response.response = "3B 8F 80 01 80 4F 0C A0 00 00 03 06 11 00 3B 00 00 00 00 42" ;
		   	return response;



		}
	LOG_INFO << "SCardReconnect called";
	if ((resp = SCardReconnect(hCard_, SCARD_SHARE_DIRECT, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_UNPOWER_CARD, &dwProtocol)) != SCARD_S_SUCCESS) {
		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			s1 = errorToString(resp) ;
			LOG_DEBUG <<"fail reconnect :" << s1 ;
			resp = handleRetry();
			LOG_INFO << "[Retry] SCardReconnect called";
			resp = SCardReconnect(hCard_, SCARD_SHARE_DIRECT, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_UNPOWER_CARD, &dwProtocol);
			tries++;
		}
		if (resp != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call SCardReconnect() "
					  << "[hCard_:" << hCard_ << "][[dwShareMode:" << SCARD_SHARE_DIRECT << "]"
					  << "[dwActiveProtocol:" << dwActiveProtocol_ << "]";
			return handleErrorResponse("Failed to cold reset", resp);
		}
	}

	switch (dwActiveProtocol_) {
	case SCARD_PROTOCOL_T0:
		pioSendPci_ = *SCARD_PCI_T0;
		break;
	case SCARD_PROTOCOL_T1:
		pioSendPci_ = *SCARD_PCI_T1;
		break;
	}

	BYTE bAttr[32];
	DWORD cByte = 32;
	response = retrieveAtr(bAttr, &cByte);
	response.response = utils::unsignedCharToString(bAttr, cByte);
	return response;
}

/*
ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::warmReset() {
	ResponsePacket response;
	LONG resp;
	DWORD dwProtocol;

	int tries = 0;
	LOG_INFO << "SCardReconnect called";
	if ((resp = SCardReconnect(hCard_, SCARD_SHARE_DIRECT, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_RESET_CARD, &dwProtocol)) != SCARD_S_SUCCESS) {
		while (resp != SCARD_S_SUCCESS && tries < TRIES_LIMIT) {
			resp = handleRetry();
			LOG_INFO << "[Retry] SCardReconnect called";
			resp = SCardReconnect(hCard_, SCARD_SHARE_DIRECT, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_RESET_CARD, &dwProtocol);
			tries++;
		}
		if (resp != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call SCardReconnect() "
					  << "[hCard_:" << hCard_ << "][[dwShareMode:" << SCARD_SHARE_DIRECT << "]"
					  << "[dwActiveProtocol:" << dwActiveProtocol_ << "]";
			return handleErrorResponse("Failed to warm reset", resp);
		}
	}

	switch (dwActiveProtocol_) {
	case SCARD_PROTOCOL_T0:
		pioSendPci_ = *SCARD_PCI_T0;
		break;
	case SCARD_PROTOCOL_T1:
		pioSendPci_ = *SCARD_PCI_T1;
		break;
	}

	BYTE bAttr[32];
	DWORD cByte = 32;
	response = retrieveAtr(bAttr, &cByte);
	response.response = utils::unsignedCharToString(bAttr, cByte);
	return response;
}
*/
ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::warmReset() {
	LOG_DEBUG << "Contactless_IDENTIV Start WarmReset";

	ResponsePacket response;
	//unsigned char command[] = { 0x96,  0x00};
	//DWORD commandLen = sizeof command;

/*
	LOG_DEBUG << "Contactless_IDENTIV Start PowerOFFField";

	LOG_DEBUG << "Set power OFF Field: ";
	// get initial polling

	if (sendEscapeCommand(command, &commandLen) != 0x00) {
		LOG_DEBUG << "Set Power OFF Field: failed";
		response.response = "Not supported";
		return response;
	}

	if ((commandLen < 2) | (command[0x00]!=0x90) | (command[0x01]!=0x00) ){
		LOG_DEBUG << "Set Power OFF Field failed: " << "Answer : " << command << " & length is : " << commandLen;
		response.response = "Failed to Set Power OFF Field, wrong answer from the reader";
		return response;

	}

	LOG_DEBUG << "Set Power OFF Field: Success";
*/
	response = powerOFFField();
/*
	if (response!="OK") {
		return response;
	}
*/
//	Sleep(100);

	response = powerONField();
/*
	LOG_DEBUG << "Start Set power ON Field: ";
	commandLen = 0x00;
	command[commandLen++] = 0x96;
	command[commandLen++] = 0x01;

	if (sendEscapeCommand(command, &commandLen) != 0x00) {
		LOG_DEBUG << "Set Power ON Field: failed";
		response.response = "Not supported";
		return response;
	}

	if ((commandLen < 2) | (command[0x00]!=0x90) | (command[0x01]!=0x00) ){
		LOG_DEBUG << "Set Power ON Field failed: " << "Answer : " << command << " & length is : " << commandLen;
		response.response = "Failed to Set Power ON Field, wrong answer from the reader";
		return response;

	}

	LOG_DEBUG << "Set power ON Success";
*/
	return response;

}

std::string ExampleTerminalPCSCContactless_IDENTIV::errorToString(LONG error) {
	switch (error) {
	case ERROR_INVALID_PARAMETER:
		return "The parameter is incorrect.";
	case ERROR_INVALID_HANDLE:
		return "The handle is invalid. Try to manually reconnect the reader.";
	case ERROR_BROKEN_PIPE:
		return "The client attempted a smart card operation in a remote session, such as a client session running on a terminal server, and the operating system in use does not support smart card redirection";
	case SCARD_E_BAD_SEEK:
		return "An error occurred in setting the smart card file object pointer";
	case SCARD_E_CANCELLED:
		return "The action was canceled by an case SCARDCancel request";
	case SCARD_E_CANT_DISPOSE:
		return "The system could not dispose of the media in the requested manner";
	case SCARD_E_CARD_UNSUPPORTED:
		return "The smart card does not meet minimal requirements for support";
	case SCARD_E_CERTIFICATE_UNAVAILABLE:
		return "The requested certificate could not be obtained";
	case SCARD_E_COMM_DATA_LOST:
		return "A communications error with the smart card has been detected. ";
	case SCARD_E_DIR_NOT_FOUND:
		return "The specified directory does not exist in the smart card";
	case SCARD_E_DUPLICATE_READER:
		return "The reader driver did not produce a unique reader name";
	case SCARD_E_FILE_NOT_FOUND:
		return "The specified file does not exist in the smart card";
	case SCARD_E_ICC_CREATEORDER:
		return "The requested order of object creation is not supported";
	case SCARD_E_ICC_INSTALLATION:
		return "No primary provider can be found for the smart card";
	case SCARD_E_INSUFFICIENT_BUFFER:
		return "The data buffer for returned data is too small for the returned data";
	case SCARD_E_INVALID_ATR:
		return "An ATR string obtained from the registry is not a valid ATR string";
	case SCARD_E_INVALID_CHV:
		return "The supplied PIN is incorrect";
	case SCARD_E_INVALID_HANDLE:
		return "The supplied handle was not valid";
	case SCARD_E_INVALID_PARAMETER:
		return "One or more of the supplied parameters could not be properly interpreted";
	case SCARD_E_INVALID_TARGET:
		return "Registry startup information is missing or not valid";
	case SCARD_E_INVALID_VALUE:
		return "One or more of the supplied parameter values could not be properly interpreted";
	case SCARD_E_NO_ACCESS:
		return "Access is denied to the file";
	case SCARD_E_NO_DIR:
		return "The supplied path does not represent a smart card directory";
	case SCARD_E_NO_FILE:
		return "The supplied path does not represent a smart card file";
	case SCARD_E_NO_KEY_CONTAINER:
		return "The requested key container does not exist on the smart card";
	case SCARD_E_NO_MEMORY:
		return "Not enough memory available to complete this command";
	case SCARD_E_NO_READERS_AVAILABLE:
		return "No smart card reader is available";
	case SCARD_E_NO_SERVICE:
		return "The smart card resource manager is not running";
	case SCARD_E_NO_SMARTCARD:
		return "The operation requires a smart card, but no smart card is currently in the device";
	case SCARD_E_NO_SUCH_CERTIFICATE:
		return "The requested certificate does not exist";
	case SCARD_E_NOT_READY:
		return "The reader or card is not ready to accept commands";
	case SCARD_E_NOT_TRANSACTED:
		return "An attempt was made to end a nonexistent transaction";
	case SCARD_E_PCI_TOO_SMALL:
		return "The PCI receive buffer was too small";
	case SCARD_E_PROTO_MISMATCH:
		return "The requested protocols are incompatible with the protocol currently in use with the card";
	case SCARD_E_READER_UNAVAILABLE:
		return "The specified reader is not currently available for use";
	case SCARD_E_READER_UNSUPPORTED:
		return "The reader driver does not meet minimal requirements for support";
	case SCARD_E_SERVER_TOO_BUSY:
		return "The smart card resource manager is too busy to complete this operation";
	case SCARD_E_SERVICE_STOPPED:
		return "The smart card resource manager has shut down";
	case SCARD_E_SHARING_VIOLATION:
		return "The smart card cannot be accessed because of other outstanding connections";
	case SCARD_E_SYSTEM_CANCELLED:
		return "The action was canceled by the system, presumably to log off or shut down";
	case SCARD_E_TIMEOUT:
		return "The user-specified time-out value has expired";
	case SCARD_E_UNEXPECTED:
		return "An unexpected card error has occurred";
	case SCARD_E_UNKNOWN_CARD:
		return "The specified smart card name is not recognized";
	case SCARD_E_UNKNOWN_READER:
		return "The specified reader name is not recognized";
	case SCARD_E_UNKNOWN_RES_MNG:
		return "An unrecognized error code was returned";
	case SCARD_E_UNSUPPORTED_FEATURE:
		return "This smart card does not support the requested feature";
	case SCARD_E_WRITE_TOO_MANY:
		return "An attempt was made to write more data than would fit in the target object";
	case SCARD_F_COMM_ERROR:
		return "An internal communications error has been detected";
	case SCARD_F_INTERNAL_ERROR:
		return "An internal consistency check failed";
	case SCARD_F_UNKNOWN_ERROR:
		return "An internal error has been detected, but the source is unknown";
	case SCARD_F_WAITED_TOO_LONG:
		return "An internal consistency timer has expired";
	case SCARD_P_SHUTDOWN:
		return "The operation has been aborted to allow the server application to exit";
	case SCARD_S_SUCCESS:
		return "No error was encountered";
	case SCARD_W_CANCELLED_BY_USER:
		return "The action was canceled by the user";
	case SCARD_W_CACHE_ITEM_NOT_FOUND:
		return "The requested item could not be found in the cache";
	case SCARD_W_CACHE_ITEM_STALE:
		return "The requested cache item is too old and was deleted from the cache";
	case SCARD_W_CARD_NOT_AUTHENTICATED:
		return "No PIN was presented to the smart card";
	case SCARD_W_CHV_BLOCKED:
		return "The card cannot be accessed because the maximum number of PIN entry attempts has been reached";
	case SCARD_W_EOF:
		return "The end of the smart card file has been reached";
	case SCARD_W_REMOVED_CARD:
		return "The smart card has been removed, so further communication is not possible";
	case SCARD_W_RESET_CARD:
		return "The smart card was reset";
	case SCARD_W_SECURITY_VIOLATION:
		return "Access was denied because of a security violation";
	case SCARD_W_UNPOWERED_CARD:
		return "Power has been removed from the smart card, so that further communication is not possible";
	case SCARD_W_UNRESPONSIVE_CARD:
		return "The smart card is not responding to a reset";
	case SCARD_W_UNSUPPORTED_CARD:
		return "The reader cannot communicate with the card, due to ATR string configuration conflicts";
	case SCARD_W_WRONG_CHV:
		return "The card cannot be accessed because the wrong PIN was presented";
	default:
		return "Unknown error";
	}
}

ResponsePacket ExampleTerminalPCSCContactless_IDENTIV::handleErrorResponse(std::string context_message, LONG error) {
	std::string message = context_message + ": " + errorToString(error);
	ResponsePacket response = { .response = "KO", .err_terminal_code = error, .err_terminal_description = message };
	return response;
}

LONG ExampleTerminalPCSCContactless_IDENTIV::handleRetry() {
	LONG response;

	if (hCard_){
		if ((response = SCardReconnect(hCard_, SCARD_SHARE_DIRECT, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_LEAVE_CARD, &dwActiveProtocol_)) == SCARD_S_SUCCESS) {
			LOG_DEBUG << "SCardReconnect() Success";
			return response;
		}
		LOG_DEBUG << "SCardReconnect() Failed";

	}

	if (hContext_){
		if (SCardReleaseContext(hContext_) != SCARD_S_SUCCESS) {
			LOG_DEBUG << "Failed to call SCardReleaseContext() " << "[card:" << hCard_ << "][hContext:" << hContext_ << "]";
		} else {
			LOG_DEBUG << "SCardReleaseContext() Success";
		}

	}
	if (SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext_) == SCARD_S_SUCCESS){
		LOG_DEBUG << "SCardEstablishContect() Success" ;

	} else {
		LOG_DEBUG << "Failed to call SCardEstablishContect() " ;
	}

	response = SCardConnect(hContext_, current_reader_.c_str(), SCARD_SHARE_DIRECT, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard_, &dwActiveProtocol_);

	return response;
}

} /* namespace client */
