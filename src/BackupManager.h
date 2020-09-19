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
 * BackupManager.h
 *
 *  Created on: 20 Feb 2012
 *      Author: alanb
 */

#ifndef BACKUPMANAGER_H_
#define BACKUPMANAGER_H_

#include "tbx/path.h"
#include <vector>

/**
 * Class to remember any backups made by the user in
 * PackMan to allow them to maintain them in PackMan
 * itself.
 */
class BackupManager
{
public:
	struct BackupItem
	{

		tbx::Path path;
		tbx::UTCTime when;
	};

private:
	std::vector<BackupItem> _items;
	typedef std::vector<BackupItem>::iterator iterator;

public:
	BackupManager();
	virtual ~BackupManager();

	void rollback();
	void commit();

	void add(const tbx::Path &path);
	void remove(const tbx::Path &path);

	typedef std::vector<BackupItem>::const_iterator const_iterator;

	/**
	 * Get constant iterator to first backup item
	 *
	 * @returns const iterator to first backup item
	 */
	const_iterator cbegin() const {return _items.begin();}

	/**
	 * Get constant iterator to end of backup items
	 *
	 * @returns const iterator to item after last
	 */
	const_iterator cend() const {return _items.end();}

	static tbx::Path get_backup_dir(const tbx::Path &target);

private:
	iterator find(const tbx::Path &path);
};

#endif /* BACKUPMANAGER_H_ */
