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

#include "VerifyFailedWindow.h"
#include "MissingFilesWindow.h"
#include "Commands.h"

#include "tbx/application.h"
#include "tbx/actionbutton.h"
#include "tbx/scrolllist.h"
#include "tbx/deleteonhidden.h"
#include "tbx/path.h"
#include "tbx/stringutils.h"

#include <sstream>


#include <iostream>
/**
 * Build information required for conflict manager and show
 * the window.
 */
VerifyFailedWindow::VerifyFailedWindow(const std::vector<Verify::FailedPackage> &failures) :
  _window("VerifyFail"),
  _show_files(this, &VerifyFailedWindow::show_files),
  _how_to_fix(this, &VerifyFailedWindow::how_to_fix)
{
	tbx::ScrollList list = _window.gadget(0);

	for (std::vector<Verify::FailedPackage>::const_iterator i = failures.begin(); i != failures.end(); ++i)
	{
		std::string package = i->first;
		package += " - " +tbx::to_string(i->second.size())+" file(s) missing";
		list.add_item(package);
		package = "** Package: " + package;
		_missing_files.push_back(package);
		for (std::vector<std::string>::const_iterator fi = i->second.begin(); fi != i->second.end(); ++fi)
		{
			_missing_files.push_back(*fi);
		}
	}

	tbx::ActionButton show_but = _window.gadget(1);
	tbx::ActionButton how_to_but = _window.gadget(3);

	show_but.add_select_command(&_show_files);
	how_to_but.add_select_command(&_how_to_fix);

	// Delete window when it has been hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<VerifyFailedWindow>(this));
}

/**
 * Delete toolbox object as this class is deleted
 */
VerifyFailedWindow::~VerifyFailedWindow()
{
	_window.delete_object();
}

/**
 * Show the full list of files
 */
void VerifyFailedWindow::show_files()
{
	new MissingFilesWindow(_missing_files);
}

/**
 * Start backup and retry of the commit
 */
void VerifyFailedWindow::how_to_fix()
{
	tbx::app()->os_cli("Filer_Run <PackMan$Dir>.html.verifyfix/htm");
}
