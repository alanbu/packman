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
 * ConflictManager.h
 *
 *  Created on: 24 May 2012
 *      Author: alanb
 */

#ifndef CONFLICTMANAGER_H_
#define CONFLICTMANAGER_H_

#include "tbx/window.h"
#include "tbx/command.h"
#include <set>
#include <vector>

/**
 * Class to give a graphical way to handle conflicts
 * reported by the commit process
 */
class ConflictManager
{
	tbx::Window _window;
	tbx::CommandMethod<ConflictManager> _show_files;
	tbx::CommandMethod<ConflictManager> _backup_and_retry;

	struct ConflictItem
	{
		std::string path_name;
		bool is_directory;
		std::vector<std::string> children;
	};

	std::vector<ConflictItem> _conflicts;

public:
	ConflictManager(const std::set<std::string> &pathnames);
	virtual ~ConflictManager();

	void show_files();
	void backup_and_retry();
};

#endif /* CONFLICTMANAGER_H_ */
