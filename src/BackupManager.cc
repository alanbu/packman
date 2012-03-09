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
 * BackupManager.cc
 *
 *  Created on: 20 Feb 2012
 *      Author: alanb
 */

#include "BackupManager.h"
#include <fstream>

const char* BackupPaths="<Choices$Write>.PackMan.Backups";
const char* BackupPathsTemp="<Choices$Write>.PackMan.++Backups";

BackupManager::BackupManager()
{
	rollback();
}

BackupManager::~BackupManager()
{
}


/**
 * To match LibPkg we use rollback to load the items
 * from file
 */
void BackupManager::rollback()
{
	std::ifstream in(BackupPaths);
	if (in)
	{
		_items.clear();
		int version;
		in >> version; // Version in case we want to change it in future
		if (version > 1) return; // Can't support future versions
		while (in)
		{
			std::string name;
			long long csec;
			in >> name;
			in >> csec;
			if (!name.empty())
			{
				BackupItem item;
				item.path = name;
				item.when = tbx::UTCTime(csec);
				_items.push_back(item);
			}
		}
	}
}

/**
 * To match LibPkg we use commit to save the backup list to
 * a file.
 */
void BackupManager::commit()
{
	int version = 1;
	tbx::Path tmp_file(BackupPathsTemp);
	tmp_file.remove();
	std::ofstream out(BackupPathsTemp);

	if (out)
	{
		out << version << std::endl;
		for (iterator i = _items.begin(); i != _items.end(); ++i)
		{
			out << i->path.name();
			out << " " << i->when.centiseconds() << std::endl;
		}
		if (out.good())
		{
			out.close();
			tmp_file.move(BackupPaths, tbx::Path::COPY_FORCE);
		}
	}
}

/**
 * Add a path to the backup manager.
 *
 * The backup manager also stores the time it was added
 */
void BackupManager::add(const tbx::Path &path)
{
	iterator found = find(path);
	if (found == _items.end())
	{
		BackupItem bi;
		bi.path = path;
		bi.when = tbx::UTCTime::now();
		_items.push_back(bi);
	} else
	{
		// New version replacing older one that hadn't been deleted
		found->when = tbx::UTCTime::now();
	}
}

/**
 * Remove a path from the backup manager
 */
void BackupManager::remove(const tbx::Path &path)
{
	iterator found = find(path);
	if (found != _items.end()) _items.erase(found);
}

/**
 * Find item with the given path
 *
 * @param path item to find
 *
 * @returns iterator to found position or _items.end()
 */
BackupManager::iterator BackupManager::find(const tbx::Path &path)
{
	iterator i;
	for (i = _items.begin(); i != _items.end(); ++i)
	{
		if (i->path.canonical_equals(path)) break;
	}

	return i;
}
