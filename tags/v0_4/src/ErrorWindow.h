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
 * ErrorWindow.h
 *
 *  Created on: 11 Feb 2010
 *      Author: alanb
 */

#ifndef ERRORWINDOW_H_
#define ERRORWINDOW_H_

#include "tbx/window.h"

/**
 * Class to display and Error in a window
 */
class ErrorWindow
{
	tbx::Window _window;

	void init(const std::string &errmsg, const std::string &title);

public:
	ErrorWindow(std::string errmsg, std::string title);
	ErrorWindow(const char *prefix, std::string errmsg, std::string title);
	virtual ~ErrorWindow();
};

#endif /* ERRORWINDOW_H_ */
