/*********************************************************************
* Copyright 2012 Alan Buckley
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
 * MoveExistsWindow.h
 *
 *  Created on: 17 Feb 2012
 *      Author: alanb
 */

#ifndef MOVEEXISTSWINDOW_H_
#define MOVEEXISTSWINDOW_H_

#include "tbx/deleteonhidden.h"
#include "tbx/command.h"
#include "tbx/path.h"

class MoveExistsWindow : public tbx::DeleteObjectOnHidden
{
	class MoveWindowCommand : public tbx::Command
	{
		std::string _logical_path;
		tbx::Path _app_path;
		std::string _to_path;
	public:
		MoveWindowCommand(const std::string &logical_path, const tbx::Path &app_path, const std::string &to_path);
		virtual void execute();
	} _yes_command;

public:
	MoveExistsWindow(const std::string &logical_path, const tbx::Path &app_path, const std::string &to_path);
	virtual ~MoveExistsWindow();
};

#endif /* MOVEEXISTSWINDOW_H_ */
