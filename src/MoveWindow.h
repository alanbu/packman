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
 * MoveWindow.h
 *
 *  Created on: 26 Jan 2012
 *      Author: alanb
 */

#ifndef MOVEWINDOW_H_
#define MOVEWINDOW_H_

#include "tbx/command.h"
#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/slider.h"
#include "tbx/actionbutton.h"
#include "MoveApp.h"

class MoveWindow : public tbx::Command
{
	tbx::Window _window;
	tbx::DisplayField _status_text;
	tbx::Slider _progress;
	tbx::ActionButton _cancel;
	tbx::CommandMethod<MoveWindow> _do_cancel;

	MoveApp _move_app;
	MoveApp::State _last_state;

	bool _can_cancel;

public:
	MoveWindow(const std::string &logical_path, const tbx::Path &app_path, const std::string &to_path);
	virtual void execute();

private:
	void cancel();
	void close();
	void show_warning();
	void show_error();

	std::string warning_text();
	std::string error_text();
};

#endif /* MOVEWINDOW_H_ */
