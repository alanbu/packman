/*********************************************************************
* Copyright 2014 Alan Buckley
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
 * ChoicesWindow.h
 *
 *  Created on: 6 Aug 2014
 *      Author: alanb
 */

#ifndef CHOICESWINDOW_H_
#define CHOICESWINDOW_H_

#include "tbx/abouttobeshownlistener.h"
#include "tbx/buttonselectedlistener.h"
#include "tbx/window.h"
#include "tbx/stringset.h"
#include "tbx/optionbutton.h"

/**
 * Dialog to allow
 */
class ChoicesWindow :
		tbx::AboutToBeShownListener,
		tbx::ButtonSelectedListener
{
	tbx::Window _window;
	tbx::StringSet _update_list_prompt;
	tbx::OptionButton _enable_logging;
	static ChoicesWindow* _instance;

public:
	ChoicesWindow();
	virtual ~ChoicesWindow();

	static void show();

private:
	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void button_selected(tbx::ButtonSelectedEvent &event);
};

#endif /* CHOICESWINDOW_H_ */
