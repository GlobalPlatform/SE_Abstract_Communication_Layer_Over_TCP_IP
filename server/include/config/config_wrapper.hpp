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

#ifndef SRC_CONFIG_WRAPPER_HPP_
#define SRC_CONFIG_WRAPPER_HPP_

#include <string>
#include "nlohmann/json.hpp"

namespace server {

// singleton pattern
class ConfigWrapper { // https://stackoverflow.com/a/1008289
public:
	static ConfigWrapper& getInstance() {
		static ConfigWrapper instance;
		return instance;
	}

	/**
	 * init - retrieve the json from the given configuration file.
	 * @param path the configuration file.
	 */
	void init(std::string path);

	/**
	 * getValue - retrieve the value at the given key.
	 * @param key the key where the data is meant to be.
	 * @return the retrieved value.
	 */
	std::string getValue(std::string key);

	/**
	 * getValue - retrieve the value at the given key or the default value if the key don't exist.
	 * @param key the key where the data is meant to be.
	 * @param default_value the return value if the key is not found.
	 * @return the retrieved value or the given default value.
	 */
	std::string getValue(std::string key, std::string default_value);
private:
	ConfigWrapper() {}
	nlohmann::json config_;
public:
	ConfigWrapper(ConfigWrapper const&) = delete;
	void operator=(ConfigWrapper const&) = delete;
};

} /* namespace client */

#endif /* SRC_CONFIG_WRAPPER_HPP_ */
