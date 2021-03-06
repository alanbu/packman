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
 * BackupWindow.cc
 *
 *  Created on: 24 Feb 2012
 *      Author: alanb
 */

#include "BackupWindow.h"
#include "BackupManager.h"

#include "tbx/deleteonhidden.h"
#include "tbx/messagewindow.h"
#include "tbx/questionwindow.h"
#include "tbx/fileraction.h"
#include "tbx/hourglass.h"
#include "tbx/application.h"

#include <cstdlib>
#include <stack>

BackupWindow *BackupWindow::_instance = 0;


/**
 * Users filer to open a directory in the desktop
 *
 * @param dir_path path to directory to open
 * @returns false if path no longer exists
 */
inline bool filer_opendir(const tbx::Path &dir_path)
{
	if (dir_path.exists())
	{
		std::string cmd("Filer_OpenDir ");
		cmd += dir_path.parent().name();
		try
		{
		   tbx::app()->os_cli(cmd);
		} catch(...)
		{
		   // Filer reports if the file is missing and no
		   // error is returned.
		}
		return true;
	} else
	{
		return false;
	}
}


/**
 * Class to Check if the delete succeeded and delete the parent
 * directory if it is empty.
 */
class CheckDeleteAfterAction : public tbx::FilerActionFinishedListener
{
	tbx::Path _deleted_path;
public:
	CheckDeleteAfterAction(tbx::Path deleted_path) : _deleted_path(deleted_path) {}

	/**
	 * Filer action finished so check the file/application was deleted.
	 */
	virtual void fileraction_finished()
	{
		if (_deleted_path.exists())
		{
			filer_opendir(_deleted_path.parent());
			tbx::show_message("Deletion of:\n"
					+ _deleted_path.name()
					+ "\nfailed. You will have to remove it by hand.",
					"",
					"warning"
					);
		} else
		{
			tbx::Path containing_dir = _deleted_path.parent();
			if (containing_dir.begin() == containing_dir.end())
			{
				containing_dir.remove();
			}
		}
		// Self destruct after check/delete
		delete this;
	}
};

/**
 * Class to check if the restore succeeded and delete the parent
 * directory if it is empty.
 */
class CheckRestoreAfterAction : public tbx::FilerActionFinishedListener
{
	tbx::Path _restored_path;
public:
	CheckRestoreAfterAction(tbx::Path restored_path) : _restored_path(restored_path) {}

	/**
	 * Filer action finished so check the file/application was deleted.
	 */
	virtual void fileraction_finished()
	{
		tbx::Path containing_dir = _restored_path.parent();
		tbx::Path target_path = containing_dir.parent().child(_restored_path.leaf_name());

		if (!target_path.exists())
		{
			filer_opendir(containing_dir);
			tbx::show_message("Failed to restore:\n"
					+ target_path.name() +
					+ "\nfrom backup at:\n"
					+ _restored_path.name()
					+ "\nYou will need to restore it by hand",
					"",
					"error"
					);
		} else if (_restored_path.exists())
		{
			filer_opendir(containing_dir);
			tbx::show_message("Failed to delete backup at:\n"
					+ _restored_path.name()
					+ "\nThis could also mean the restoration to:\n"
					+ target_path.name()
					+ "\nalso failed.\n"
					"You will need to check the application has restored OK by hand\n"
					"and delete the backup manually.",
					"",
					"warning"
					);
		} else
		{
			if (containing_dir.begin() == containing_dir.end())
			{
				containing_dir.remove();
			}
		}
		// Self destruct after check/delete
		delete this;
	}
};

/**
 * Construct the backup window creating the toolbox window
 * and setting up gadgets and listeners.
 */
BackupWindow::BackupWindow() : _window("Backups"),
	_view(this, &BackupWindow::view_backup),
	_delete(this, &BackupWindow::delete_backup),
	_restore(this, &BackupWindow::restore_backup),
	_do_delete(this, &BackupWindow::do_delete),
	_do_restore(this, &BackupWindow::do_restore)
{
	_instance = this;

	_backups = _window.gadget(1);
	_backups.add_selection_listener(this);
	_window.add_about_to_be_shown_listener(this);
	_view_button = _window.gadget(2);
	_view_button.add_select_command(&_view);
	_delete_button = _window.gadget(7);
	_delete_button.add_select_command(&_delete);
	_restore_button = _window.gadget(3);
	_restore_button.add_select_command(&_restore);

	// Dispose of object when it is hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<BackupWindow>(this));
	_window.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
}

BackupWindow::~BackupWindow()
{
	_instance = 0;
}

/**
 * Show the backup window. If it is already shown bring to the
 * top.
 */
void BackupWindow::show()
{
	if (_instance) _instance->_window.show();
	else new BackupWindow();
}

/**
 * Load current list of sources as window is shown
 */
void BackupWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	BackupManager bus;
	std::string view_item;

	_backups.clear();

	for (BackupManager::const_iterator i = bus.cbegin(); i != bus.cend(); ++i)
	{
		const tbx::Path &path = i->path;
		_paths.push_back(path);
		view_item = path.leaf_name();
		view_item += " ";
		view_item += (i->when).text();
		_backups.add_item(view_item);
	}
}

/**
 * Item selected on scroll list so update buttons
 */
void BackupWindow::scrolllist_selection(const tbx::ScrollListSelectionEvent &event)
{
	bool fade = (event.index() == -1);
	_view_button.fade(fade);
	_delete_button.fade(fade);
	_restore_button.fade(fade);
}

/**
 * Open folder containing backup so it can be viewed
 */
void BackupWindow::view_backup()
{
	int index = _backups.first_selected();
	if (index >= 0)
	{
		if (!filer_opendir(_paths[index]))
		{
			backup_already_deleted(index);
		}
	}
}

/**
 * Delete the backup
 */
void BackupWindow::delete_backup()
{
	int index = _backups.first_selected();
	if (index >= 0)
	{
		tbx::Path &path = _paths[index];
		if (path.exists())
		{
			tbx::show_question_as_menu("Are you sure you wish to delete the backup at\n"
					+ path.name() + "?",
					"",
					&_do_delete
					);
		} else
		{
			backup_already_deleted(index);
		}
	}
}

/**
 * Restore the backup
 */
void BackupWindow::restore_backup()
{
	int index = _backups.first_selected();
	if (index >= 0)
	{
		tbx::Path &path = _paths[index];
		if (path.exists())
		{
			tbx::Path restore_to(path.parent().parent(), path.leaf_name());
			if (check_exists(restore_to, path))
			{
				tbx::show_message("Unable to restore backup at\n"
						+ path.name()
						+ "\nThere is something already at the restore location\n"
						+ restore_to.name()
						+ "\nPlease move this out of the way first."
						"If this is an installed item, please uninstall it.",
						"",
						"error"
						);
			} else
			{
				tbx::show_question_as_menu("Are you sure you wish to restore the backup from\n"
						+ path.name()
						+"\nto\n"
						+ restore_to.name()
						+ "?",
						"",
						&_do_restore
						);
			}

		} else
		{
			backup_already_deleted(index);
		}
	}
}

/**
 * Check files in restore to location, existing directories should
 * not stop the restore.
 *
 * @param restore_to path files will be copied to
 * @param restore_from files that are going to be restored.
 */
bool BackupWindow::check_exists(const tbx::Path &restore_to, const tbx::Path &restore_from)
{
	tbx::PathInfo info;
	if (!restore_to.path_info(info)) return false;
	if (!info.directory() && !info.image_file()) return true;
	tbx::Hourglass hg;
	std::stack< std::pair<tbx::Path, tbx::Path> > dirs;
	dirs.push(std::make_pair(restore_from, restore_to));

	while (!dirs.empty())
	{
		tbx::Path from_dir = dirs.top().first;
		tbx::Path to_dir = dirs.top().second;
		dirs.pop();

		for (tbx::Path::Iterator i = from_dir.begin(); i != from_dir.end(); ++i)
		{
			tbx::Path check_path(to_dir, *i);
			if (check_path.exists())
			{
				if (!check_path.directory()) return true;
				dirs.push(std::make_pair(tbx::Path(from_dir, *i), tbx::Path(to_dir, *i)));
			}
		}
	}

	return false;
}

/**
 * Actually do deletion of selected backup
 */
void BackupWindow::do_delete()
{
	int index = _backups.first_selected();
	if (index < 0) return; // Should never happen

	tbx::Path &path = _paths[index];
	tbx::FilerAction fa(path);

	fa.remove(tbx::FilerAction::RECURSE|tbx::FilerAction::VERBOSE);
	fa.add_finished_listener(new CheckDeleteAfterAction(path));

	remove_from_list(index);
}

/**
 * Actually do restore of selected backup
 */
void BackupWindow::do_restore()
{
	int index = _backups.first_selected();
	if (index < 0) return; // Should never happen

	tbx::Path &path = _paths[index];
	tbx::Path restore_to(path.parent().parent());

	tbx::FilerAction fa(path);
	fa.move(restore_to, tbx::FilerAction::RECURSE|tbx::FilerAction::VERBOSE);

	fa.add_finished_listener(new CheckRestoreAfterAction(path));

	remove_from_list(index);
}


/**
 * Backup has already been deleted so remove it from list
 * of backups
 *
 * @param index in scroll list of backup that has been deleted
 */
void BackupWindow::backup_already_deleted(int index)
{
	tbx::Path &path = _paths[index];

	tbx::show_message("The backup at\n" + path.name()
			+ "\nno longer exists.\n"
			"\nIt has now been removed from the backup list.",
			"",
			"information"
			);
	remove_from_list(index);
}

/**
 * Remove backup from backup window list and backup manager
 *
 * @param index index of backup in scroll list
 */
void BackupWindow::remove_from_list(int index)
{
	tbx::Path path = _paths[index];

	_backups.delete_item(index);
	_paths.erase(_paths.begin() + index);

	BackupManager bus;
	bus.remove(path);
	bus.commit();
}
