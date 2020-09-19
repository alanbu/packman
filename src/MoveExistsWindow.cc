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
