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
	bool check_exists(const tbx::Path &restore_to, const tbx::Path &restore_from);

	void do_delete();
	void do_restore();
};

#endif /* BACKUPWINDOW_H_ */
