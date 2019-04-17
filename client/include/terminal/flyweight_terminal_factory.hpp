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

#ifndef TERMINAL_FLYWEIGHT_FACTORY_H_
#define TERMINAL_FLYWEIGHT_FACTORY_H_

#include "terminal/factories/factory.hpp"

#include <map>

namespace client {

class FlyweightTerminalFactory {
private:
	std::map<std::string, ITerminalFactory*> factories_;
public:
	FlyweightTerminalFactory() {};
	virtual ~FlyweightTerminalFactory() {};

	/**
	 * addFactory - add a factory in the cache at the given key.
	 * @param key the key used to store the factory.
	 * @param factory the terminal factory.
	 */
	void addFactory(std::string key, ITerminalFactory* factory);

	/**
	 * getFactory - retrieve to factory at the given key.
	 * @param key the key used to retrieve the factory.
	 * @return the factory at the given key.
	 */
	ITerminalFactory* getFactory(std::string key);
};

} /* namespace terminal */

#endif /* TERMINAL_FLYWEIGHT_FACTORY_H_ */
