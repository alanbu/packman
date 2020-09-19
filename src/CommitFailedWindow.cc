/*********************************************************************
* This file is part of PackMan
*
* Copyright 2009-2020 AlanBuckley
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
