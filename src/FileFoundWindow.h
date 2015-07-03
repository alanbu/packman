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
/*
 * FileFoundWindow.h
 *
 *  Created on: 15 May 2015
 *      Author: alanb
 */

#ifndef FILEFOUNDWINDOW_H_
#define FILEFOUNDWINDOW_H_

#include "tbx/command.h"
#include <string>
/**
 * Window to show the results of a successful component find
 */
class FileFoundWindow
{
	std::string _find_text;
	std::string _package;
	std::string _logical_path;
	tbx::CommandMethod<FileFoundWindow> _show_info;
public:
	FileFoundWindow(const std::string &find_text, const std::string &package, const std::string &logical_path, const std::string &title);
	virtual ~FileFoundWindow();

	void show_info();
};

#endif /* FILEFOUNDWINDOW_H_ */
