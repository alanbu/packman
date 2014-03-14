/*********************************************************************
* Copyright 2014 Alan Buckley
*
* This file is part of PkgTest (PackMan unit testing)
*
* PackMan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* PkgTest is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with PackMan. If not, see <http://www.gnu.org/licenses/>.
*
*****************************************************************************/
/*
 * AddTestWnd.h
 *
 *  Created on: 19 Feb 2014
 *      Author: alanb
 */

#ifndef ADDTESTWND_H_
#define ADDTESTWND_H_

#include "tbx/window.h"
#include "tbx/writablefield.h"
#include "tbx/command.h"
#include "tbx/abouttobeshownlistener.h"


/**
 * Class to handle adding a new test window
 */
class AddTestWnd : tbx::AboutToBeShownListener
{
	tbx::Window _window;
	tbx::WritableField _name_field;
	tbx::WritableField _desc_field;
	tbx::CommandMethod<AddTestWnd> _add_command;

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &e);
public:
	AddTestWnd(tbx::Object obj);
	virtual ~AddTestWnd();

	void add();
};

#endif /* ADDTESTWND_H_ */
