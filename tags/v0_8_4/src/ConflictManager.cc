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
 * ConflictManager.cc
 *
 *  Created on: 24 May 2012
 *      Author: alanb
 */

#include "ConflictManager.h"
#include "ConflictsWindow.h"
#include "PackManState.h"
#include "BackupAndRun.h"
#include "Commands.h"

#include "tbx/actionbutton.h"
#include "tbx/scrolllist.h"
#include "tbx/deleteonhidden.h"
#include "tbx/path.h"

#include <sstream>


#include <iostream>
/**
 * Build information required for conflict manager and show
 * the window.
 */
ConflictManager::ConflictManager(const std::set<std::string> &pathnames) :
  _window("ConflictMan"),
  _show_files(this, &ConflictManager::show_files),
  _backup_and_retry(this, &ConflictManager::backup_and_retry)
{
	std::string last_path;
	ConflictItem new_item;
	std::set<std::string>::const_iterator i = pathnames.begin();

	// Set up first item from the first path name
	new_item.path_name = *i;
	new_item.is_directory = false;
	_conflicts.push_back(new_item);

	++i;

	for ( ; i != pathnames.end(); ++i)
	{
		std::string path_name = *i;
		ConflictItem &conflict = _conflicts.back();
		bool add_new = true;

		if (conflict.is_directory)
		{
			std::string::size_type dir_size = conflict.path_name.size();
			// First check if it's a child of a previous folder
			if (path_name.size() > dir_size
				&& path_name[dir_size] == '.'
				&&	path_name.compare(0, dir_size, conflict.path_name) == 0
			)
			{
				conflict.children.push_back(path_name.substr(dir_size+1));
				add_new = false;
			}
		} else
		{
			std::string::size_type dir_pos = path_name.rfind('.');

			if (dir_pos != std::string::npos
				&& dir_pos < conflict.path_name.size()
				&& conflict.path_name[dir_pos] == '.'
				&& path_name.compare(0, dir_pos, conflict.path_name, 0, dir_pos) == 0
				)
			{
				// Both in same directory so change conflict to parent directory
				conflict.is_directory = true;
				conflict.children.push_back(conflict.path_name.substr(dir_pos+1));
				conflict.children.push_back(path_name.substr(dir_pos+1));
				conflict.path_name = path_name.substr(0, dir_pos);
				add_new = false;
			}
		}
		if (add_new)
		{
			new_item.path_name = path_name;
			new_item.is_directory = false;
			new_item.children.clear();
			_conflicts.push_back(new_item);
		}
	}

	tbx::ScrollList list = _window.gadget(0);

	for (std::vector<ConflictItem>::iterator ci = _conflicts.begin(); ci != _conflicts.end(); ++ci)
	{
		ConflictItem &conflict = *ci;
		tbx::Path path(conflict.path_name);
		std::ostringstream ss;
		ss << path.leaf_name();
		if (conflict.is_directory)
		{
			ss << " (" << conflict.children.size() << " children)";
		}
		ss << " at " << path.parent().name();
		list.add_item(ss.str());
	}

	tbx::ActionButton show_but = _window.gadget(1);
	tbx::ActionButton retry_but = _window.gadget(3);

	show_but.add_select_command(&_show_files);
	retry_but.add_select_command(&_backup_and_retry);

	// Delete window when it has been hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<ConflictManager>(this));
}

/**
 * Delete toolbox object as this class is deleted
 */
ConflictManager::~ConflictManager()
{
	_window.delete_object();
}

/**
 * Show the full list of files
 */
void ConflictManager::show_files()
{
	std::set<std::string> path_names;
	for (std::vector<ConflictItem>::iterator ci = _conflicts.begin(); ci != _conflicts.end(); ++ci)
	{
		ConflictItem &conflict = *ci;
		if (conflict.is_directory)
		{
			for (std::vector<std::string>::iterator i = conflict.children.begin();
					i != conflict.children.end(); ++i)
			{
				std::string full_name = conflict.path_name + "." + *i;
				path_names.insert(full_name);
			}
		} else
		{
			path_names.insert(conflict.path_name);
		}
	}

	new ConflictsWindow(path_names);
}

/**
 * Start backup and retry of the commit
 */
void ConflictManager::backup_and_retry()
{
	if (pmstate()->ok_to_commit())
	{
		BackupAndRun *bur = new BackupAndRun("Backing up conflicts and retrying", new CommitCommand());
		for (std::vector<ConflictItem>::iterator ci = _conflicts.begin(); ci != _conflicts.end(); ++ci)
		{
			ConflictItem &conflict = *ci;
			if (conflict.is_directory)
			{
				bur->add_children(conflict.path_name, conflict.children);
			} else
			{
				bur->add(conflict.path_name);
			}
		}
		bur->start();
	}
}
