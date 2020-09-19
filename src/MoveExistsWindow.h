/*********************************************************************
* This file is part of PackMan
*
* Copyright 2012-2020 AlanBuckley
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
