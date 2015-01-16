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
 * MoveExistsWindow.cc
 *
 *  Created on: 17 Feb 2012
 *      Author: alanb
 */

#include "MoveExistsWindow.h"
#include "MoveWindow.h"
#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/actionbutton.h"

/**
 * Construct and show the MoveExists window
 */
MoveExistsWindow::MoveExistsWindow(const std::string &logical_path, const tbx::Path &app_path, const std::string &to_path) :
		_yes_command(logical_path, app_path, to_path)
{
	tbx::Window move_exists("MoveExists");
	tbx::DisplayField app_display = move_exists.gadget(2);
	tbx::DisplayField path_display = move_exists.gadget(4);
	tbx::ActionButton yes_button = move_exists.gadget(8);
	tbx::Path tp(to_path);

	app_display.text(tp.leaf_name());
	path_display.text(tp.parent());

	yes_button.add_select_command(&_yes_command);

	// Delete this and toolbox object when hidden
	move_exists.add_has_been_hidden_listener(this);

	// Finally show the window
	move_exists.show();
}

MoveExistsWindow::~MoveExistsWindow()
{
}

/**
 * Construct the command to show the move window when "yes" is clicked
 */
MoveExistsWindow::MoveWindowCommand::MoveWindowCommand(const std::string &logical_path, const tbx::Path &app_path, const std::string &to_path) :
		_logical_path(logical_path),
		_app_path(app_path),
		_to_path(to_path)
{
}

/**
 * Execute the move window to actually backup and move the application
 */
void MoveExistsWindow::MoveWindowCommand::execute()
{
	new MoveWindow(_logical_path, _app_path, _to_path);
}
