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
 * BackupWindow.h
 *
 *  Created on: 24 Feb 2012
 *      Author: alanb
 */

#ifndef BACKUPWINDOW_H_
#define BACKUPWINDOW_H_

#include "tbx/abouttobeshownlistener.h"
#include "tbx/window.h"
#include "tbx/scrolllist.h"
#include "tbx/command.h"
#include "tbx/actionbutton.h"
#include "tbx/optionbutton.h"
#include "tbx/path.h"

#include <vector>

/**
 * Class to show and allow maintenance of Backups
 * done by PackMan
 */
class BackupWindow  :
	tbx::AboutToBeShownListener,
	tbx::ScrollListSelectionListener
{
	tbx::Window _window;
	tbx::ScrollList _backups;
	tbx::CommandMethod<BackupWindow> _view, _delete, _restore, _do_delete, _do_restore;
	tbx::ActionButton _view_button, _delete_button, _restore_button;

	std::vector<tbx::Path> _paths;

	static BackupWindow *_instance;

	// Event processing
	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void scrolllist_selection(const tbx::ScrollListSelectionEvent &event);

	// Constructor private as we only allow a single instance shown
	// using show
	BackupWindow();

public:
	virtual ~BackupWindow();

	static void show();

	// Implement commands on window
	void view_backup();
	void delete_backup();
	void restore_backup();

private:
	// Helpers
	void backup_already_deleted(int index);
	void remove_from_list(int index);

	void do_delete();
	void do_restore();
};

#endif /* BACKUPWINDOW_H_ */
