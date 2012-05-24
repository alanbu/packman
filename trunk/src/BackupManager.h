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
