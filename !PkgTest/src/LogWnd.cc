/*********************************************************************
* Copyright 2014 Alan Buckley
*
* This file is part of PkgTest (PackMan unit testing)
*
* PackMan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* PkgTest is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with PackMan. If not, see <http://www.gnu.org/licenses/>.
*
*****************************************************************************/
/*
 * LogWnd.cc
 *
 *  Created on: 14 Feb 2014
 *      Author: alanb
 */

#include "LogWnd.h"

#include <fstream>

LogWnd::LogWnd(const std::string &log_path) :
	_view_value(_lines),
	_renderer(&_view_value),
	_view(tbx::Window("Log"), &_renderer)
{
	std::string::size_type pos = log_path.rfind('.');
	std::string script = log_path.substr(pos+1);

	std::ifstream in(log_path.c_str());
	std::string line;

	while (in)
	{
		std::getline(in, line);
		_lines.push_back(line);
	}
	_view.inserted(0, _lines.size());

	_view.window().title("Log of test " + script);
	_view.window().show();

	// Self destruct on close
	_view.window().add_has_been_hidden_listener(this);
}

LogWnd::~LogWnd()
{
}

