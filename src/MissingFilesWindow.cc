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

#include "MissingFilesWindow.h"
#include "tbx/deleteonhidden.h"

MissingFilesWindow::MissingFilesWindow(const std::vector<std::string> &pathnames) :
	_window("Missing"),
	_list(_window)
{
	_list.assign(pathnames.begin(), pathnames.end());

	// Delete window when it has been hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<MissingFilesWindow>(this));
}

MissingFilesWindow::~MissingFilesWindow()
{
	_window.delete_object();
}
