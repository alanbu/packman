/*********************************************************************
* Copyright 2012 Alan Buckley
*
* This file is part of PackMan.
*
* PackMan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* PackMan is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with PackMan. If not, see <http://www.gnu.org/licenses/>.
*
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

	bool installed();
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
