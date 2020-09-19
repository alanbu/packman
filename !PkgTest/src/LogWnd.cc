/*********************************************************************
* This file is part of PkgTest (PackMan unit testing)
*
* Copyright 2014-2020 AlanBuckley
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

