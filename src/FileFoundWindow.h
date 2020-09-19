/*********************************************************************
* This file is part of PackMan
*
* Copyright 2015-2020 AlanBuckley
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
