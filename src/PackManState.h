/*********************************************************************
* This file is part of PackMan
*
* Copyright 2012-2020 AlanBuckley
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*****************************************************************************/

/*
 * PackManState.h
 *
 *  Created on: 14 May 2012
 *      Author: alanb
 */

#ifndef PACKMANSTATE_H_
#define PACKMANSTATE_H_

#include <string>

namespace tbx
{
	class Iconbar;
}

/**
 * Singleton class to monitor the current state of PackMan.
 *
 * Also can start !Packages install if necessary
 * and show a busy dialog when busy.
 */
class PackManState
{
private:
	static PackManState *_instance;
	bool _moving;

public:
	PackManState();
	~PackManState();

	static PackManState *instance() {return _instance;}

	bool installed(tbx::Iconbar *iconbar = nullptr);
	bool ok_to_commit();
	bool ok_to_move();

	void moving(bool m);

private:
	enum StateType {NONE, COMMIT, MOVE};
	void show_busy(StateType trying, StateType why_not);

	std::string state_text(StateType type);
};

/**
 * Short cut to the global PackManState instance
 */
inline PackManState *pmstate() {return PackManState::instance();}

#endif /* PACKMANSTATE_H_ */
