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
		try
		{
			if (i->path.canonical_equals(path)) break;
		}
		catch(const std::exception&)
		{
			// Ignore exception, usually caused by path no longer valid
		}
		
	}

	return i;
}

/**
 * Static function to get the location to create a backup of the
 * give file.
 */
tbx::Path BackupManager::get_backup_dir(const tbx::Path &target)
{
	tbx::Path backup_dir;
	char backup_dirname[20];
	int backup_dirnum = 0;
	do
	{
		backup_dirnum++;
		sprintf(backup_dirname, "Backup%d", backup_dirnum);
		backup_dir = target.parent();
		backup_dir.down(backup_dirname);
	} while (backup_dir.exists());

	return backup_dir;
}
