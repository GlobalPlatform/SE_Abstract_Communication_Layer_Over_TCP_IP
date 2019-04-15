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

#ifndef UTILS_TYPE_CONVERTER_H_
#define UTILS_TYPE_CONVERTER_H_

#include "plog/include/plog/Log.h"

#include <sstream>
#include <string>
#include <vector>

namespace utils {

/**
 * stringToUnsignedChar - convert a string to unsigned char*
 * By example: unsigned char bytes[] = { 0x0F, 0xFF, 0x44, 0x4A } <===> std::string s = "0F FF 44 4A"
 * @param data to string to convert.
 * @param length to outputed length of the unsigned char* result.
 * @return the unsigned char* data.
 */
inline unsigned char* stringToUnsignedChar(std::string data, unsigned long int* length) {
	data.erase(remove_if(data.begin(), data.end(), isspace), data.end()); // remove spaces

	std::stringstream ss;
	for (unsigned long int i = 0; i < data.length(); i++) {
		if (i != 0 && i % 2 == 0) {
			ss << " ";
		}
		ss << data[i];
	}
	data = ss.str();

	std::istringstream hex_chars_stream(data);
	std::vector<unsigned char> bytes;
	unsigned int c;
	int i = 0;
	while (hex_chars_stream >> std::hex >> c) {
		bytes.push_back(c);
		i++;
	}

	unsigned char* ustr = new unsigned char[bytes.size()];
	std::copy(bytes.begin(), bytes.end(), ustr);
	*length = bytes.size();
	return ustr;
}

/**
 * unsignedCharToString - convert an unsigned char* to a string
 * By example: unsigned char bytes[] = { 0x0F, 0xFF, 0x44, 0x4A } <===> std::string s = "0F FF 44 4A"
 * @param hex the unsigned char* data to be converted.
 * @param length the length of the data to be converted.
 * @return the string data.
 */
inline std::string unsignedCharToString(unsigned char* hex, unsigned long int length) {
	std::string hexString;
	char buffer[length];
	for (unsigned long int i = 0; i < length; i++) {
		if (i == length - 1) {
			sprintf(buffer, "%02X", hex[i]);
		} else {
			sprintf(buffer, "%02X ", hex[i]);
		}
		hexString.append(buffer);
	}
	return hexString;
}

} // namespace utils

#endif /* UTILS_TYPE_CONVERTER_H_ */
