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
