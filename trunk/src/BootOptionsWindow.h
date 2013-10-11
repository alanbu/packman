
/*********************************************************************
* Copyright 2013 Alan Buckley
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
 * BootOptionsWindow.h
 *
 *  Created on: 9 Oct 2013
 *      Author: alanb
 */

#ifndef BOOTOPTIONSWINDOW_H_
#define BOOTOPTIONSWINDOW_H_

#include "tbx/abouttobeshownlistener.h"
#include "tbx/command.h"
#include "tbx/window.h"
#include "tbx/optionbutton.h"

/**
 * Class to show and allow changing of the current boot options
 */
class BootOptionsWindow : tbx::AboutToBeShownListener
{
	tbx::Window _window;
	tbx::OptionButton _look_at_option;
	tbx::OptionButton _run_option;
	tbx::OptionButton _add_to_apps_option;
	tbx::CommandMethod<BootOptionsWindow> _apply_command;

	std::string _app_path;
	bool _look_at;
	bool _run;
	bool _add_to_apps;

public:
	BootOptionsWindow(tbx::Object window);
	virtual ~BootOptionsWindow();

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);

	void apply();
};

#endif /* BOOTOPTIONSWINDOW_H_ */
