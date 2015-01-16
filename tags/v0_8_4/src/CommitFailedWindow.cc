/*********************************************************************
* Copyright 2009 Alan Buckley
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
 * CommitFailedWindow.cc
 *
 *  Created on: 19-Aug-2009
 *      Author: alanb
 */

#include "CommitFailedWindow.h"
#include "Packages.h"
#include "LogViewer.h"


#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/button.h"
#include "tbx/deleteonhidden.h"

// libpkg commit.h doesn't currently define set and map and puts string
// in no namespace
#include <map>
#include <set>

using namespace std;

#include "libpkg/commit.h"
#include "libpkg/log.h"


CommitFailedWindow::CommitFailedWindow(pkg::commit *commit, std::string last_action, std::tr1::shared_ptr<pkg::log> commit_log) :
		_log(commit_log),
		_log_viewer(0),
	    _show_log_command(this, &CommitFailedWindow::show_log)
{
	tbx::Window window("CommitFail");
	tbx::DisplayField when(window.gadget(0));
	tbx::Button msg(window.gadget(1));
	msg.value(commit->message());
	when.text("Failed to install/remove package when " + last_action);

	if (_log)
	{
		tbx::ActionButton log_button(window.gadget(3));
		log_button.add_select_command(&_show_log_command);
	} else
	{
		window.remove_gadget(3);
	}

	// Dump class and object when window is hidden
	window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<CommitFailedWindow>(this));
	window.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
}

CommitFailedWindow::~CommitFailedWindow()
{
}

/**
 * Show (or reshow) the log
 */
void CommitFailedWindow::show_log()
{
	if (_log_viewer == 0) _log_viewer = new LogViewer(_log);
	_log_viewer->show();
}
