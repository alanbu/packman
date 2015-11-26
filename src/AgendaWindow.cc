/*********************************************************************
* Copyright 2015 Alan Buckley
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
/*/*
 * AgendaWindow.cc
 *
 *  Created on: 23-Jul-2009
 *      Author: alanb
 */

#include "AgendaWindow.h"

AgendaWindow::AgendaWindow() :
    _window("Agenda"),
	_list(_window)
{
}

/**
 * Populate the agenda and show the window.
 *
 * @param list the install list with what to do on.
 */
void AgendaWindow::show(const InstallList &list)
{
	_list.clear();
	std::string msg;

	if (list.paths_count())
	{
		for (auto path : list.paths())
		{
			msg = "Add Path '" + path.first + "', location '" + path.second + "'";
			_list.push_back(msg);
		}
	}

	if (!list.clash_paths().empty())
	{
		for (auto path : list.clash_paths())
		{
			msg = "Ignoring Path '" + path.first + "' as it clashes with the existing path";
			_list.push_back(msg);
		}
	}

	if (list.sources_count())
	{
		for (auto source : list.sources())
		{
			msg = "Add Source '" + source + "'";
			_list.push_back(msg);
		}
	}

	if (!list.unavailable_packages().empty())
	{
		_list.push_back("Prompt to update package list");
	}

	if (list.packages_count())
	{
		for (auto pack : list.packages())
		{
			msg = "Add Package '" +  pack.name + "'";
			_list.push_back(msg);
		}
	}

	if (!list.unavailable_packages().empty())
	{
		for (auto pack : list.unavailable_packages())
		{
			msg = "May Add Package '"+  pack.name + "' if found after update lists";
			_list.push_back(msg);
		}
	}

	_window.show();
}

AgendaWindow::~AgendaWindow()
{
	_window.delete_object();
}
