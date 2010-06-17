/*********************************************************************
* Copyright 2010 Alan Buckley
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
 * ErrorWindow.cc
 *
 *  Created on: 11 Feb 2010
 *      Author: alanb
 */

#include "ErrorWindow.h"
#include "tbx/displayfield.h"
#include "tbx/deleteonhidden.h"

/**
 * Generic error window.
 *
 */
ErrorWindow::ErrorWindow(std::string errmsg, std::string title)
{
	init(errmsg, title);
}

ErrorWindow::ErrorWindow(const char *prefix, std::string errmsg, std::string title)
{
	std::string msg(prefix);
	msg += errmsg;
	init(msg, title);
}

void ErrorWindow::init(const std::string &errmsg, const std::string &title)
{
	_window = tbx::Window("Error");
	tbx::DisplayField msg(_window.gadget(1));
	_window.title(title);
	msg.text(errmsg);

	// Dump class and object when window is hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<ErrorWindow>(this));
	_window.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
}

ErrorWindow::~ErrorWindow()
{
}
