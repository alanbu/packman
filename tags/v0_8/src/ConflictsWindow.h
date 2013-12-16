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
 * ConflictsWindow.h
 *
 *  Created on: 23-Jul-2009
 *      Author: alanb
 */

#ifndef CONFLICTSWINDOW_H_
#define CONFLICTSWINDOW_H_

#include "tbx/window.h"
#include "StringListView.h"
#include <set>

class ConflictsWindow
{
	tbx::Window _window;
	StringListView _list;

public:
	ConflictsWindow(const std::set<std::string> &pathnames);
	virtual ~ConflictsWindow();
};

#endif /* CONFLICTSWINDOW_H_ */
