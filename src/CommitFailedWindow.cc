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


#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/deleteonhidden.h"

// libpkg commit.h doesn't currently define set and map and puts string
// in no namespace
#include <map>
#include <set>

using namespace std;

#include "libpkg/commit.h"


CommitFailedWindow::CommitFailedWindow(pkg::commit *commit)
{
	tbx::Window window("CommitFail");
	tbx::DisplayField msg(window.gadget(1));
	msg.text(commit->message());

	// Dump class and object when window is higgen
	window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<CommitFailedWindow>(this));
	window.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
}

CommitFailedWindow::~CommitFailedWindow()
{
}
