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
 * BackupAndRun.cc
 *
 *  Created on: 18 May 2012
 *      Author: alanb
 */

#include "BackupAndRun.h"
#include "BackupManager.h"
#include "tbx/application.h"
#include "tbx/messagewindow.h"
#include "tbx/oserror.h"
#include <cassert>


// Number of times to poll FSCopy from one WIMP poll when running faster
const int FASTER_LOOP_COUNT = 20;


/**
 * Construct with title for window and command
 *
 * @param title Title for the window
 * @param command Command to run (this is deleted when the window is deleted).
 */
BackupAndRun::BackupAndRun(const std::string &title, tbx::Command *command) :
   _run_command(command),
   _window("Move"),
   _do_cancel(this, &BackupAndRun::cancel),
   _do_faster(this, &BackupAndRun::faster),
   _state(START_BACKUP),
   _error(NO_ERROR)
{
	_status_text = _window.gadget(1);
	_progress = _window.gadget(2);
	_cancel = _window.gadget(3);
	_cancel.add_selected_command(&_do_cancel);
	_can_cancel = true;
	_cancelled = false;

	_faster = _window.gadget(4);
	_faster.add_selected_command(&_do_faster);
	_run_faster = false;

	_window.title(title);

	_last_state = DONE; // Force poll to update display

	_cost_to_backup = 0;
	_cost_done = 0;
	_cost_total = 0;
}

BackupAndRun::~BackupAndRun()
{
	delete _run_command;
	while (!_backups.empty())
	{
		delete _backups.front();
		_backups.pop_front();
	}
	while(!_backups_done.empty())
	{
		delete _backups_done.top();
		_backups_done.pop();
	}
}

/**
 * Add a file system object to be backed up
 *
 * @param backup_object directory or file to backup
 */
void BackupAndRun::add(const tbx::Path &backup_object)
{
	tbx::Path backup_dir = BackupManager::get_backup_dir(backup_object);

	FSObjectCopy *backup_handler = new FSObjectCopy(backup_object, backup_dir);
	backup_handler->delete_option(FSObjectCopy::DELETE_AFTER_COPY);
	_backups.push_back(backup_handler);
}

/**
 * Add specified children of a directory to be backed up
 *
 * The backup will be named after the directory
 *
 * @param backup_root name of the parent directory
 * @param children list of the files to backup within that directory
 */
void BackupAndRun::add_children(const tbx::Path &backup_root, const std::vector<std::string> &children)
{
	tbx::Path backup_dir = BackupManager::get_backup_dir(backup_root);

	FSObjectCopy *backup_handler = new FSObjectCopy(backup_root, backup_dir);
	backup_handler->delete_option(FSObjectCopy::DELETE_AFTER_COPY);

	for (std::vector<std::string>::const_iterator i = children.begin(); i != children.end(); ++i)
	{
		backup_handler->add_child_file(*i);
	}

	_backups.push_back(backup_handler);

}

/**
 * Start processing added files/folders
 */
void BackupAndRun::start()
{
	assert(!_backups.empty());
	if (!_backups.empty())
	{
		_window.show();

		tbx::app()->add_idle_command(this);
	}
}


/**
 * Routine polled by application on idle events
 */
void BackupAndRun::execute()
{
	if (_cost_total)
	{
		int done = (1000 * _cost_done) / _cost_total;
		_progress.value(done);
	}

	if (_state != _last_state)
	{
		if (!_can_cancel)
		{
			_cancel.fade(_can_cancel);
			_cancel.text(_can_cancel ? "Cancel" : "Processing...");
		}

		_last_state = _state;
		switch(_state)
		{
		case START_BACKUP:
			_status_text.text("Making list of files to backup");
			break;
		case BACKUP_NEXT:
			_status_text.text("Backing up next target");
			break;
		case BACKUP_FILES:
			_status_text.text("Backing up files");
			break;
		case UNWIND_NEXT:
			_status_text.text("Unwinding next target");
			break;
		case UNWIND_BACKUP:
			_status_text.text("Unwinding after error or cancel");
			break;
		case DONE:
			_status_text.text("Finished backup - running command.");
			break;
		case FAILED:
			_status_text.text("Backup failed");
			break;
		}
	}

	if (_run_faster)
	{
		int loop = FASTER_LOOP_COUNT;
		while (loop && process()) loop--;
	} else
	{
		process();
	}
}

/**
 * Do one process
 *
 * Note: The BackupAndRun class could be deleted by the end
 * of this function.
 *
 * @returns true if more to do, false otherwise
 */
bool BackupAndRun::process()
{
	bool more = true;

	switch(_state)
	{
	case START_BACKUP:
		// Need to create target directories and calculate total cost
		{
			for (std::deque<FSObjectCopy *>::iterator i = _backups.begin();
					i != _backups.end() && _state == START_BACKUP;
					++i)
			{
				FSObjectCopy *backup = *i;
				tbx::Path backup_dir(backup->target_dir());
				try
				{
				   backup_dir.create_directory();
				} catch(tbx::OsError &e)
				{
				   _state = FAILED;
				   _errmsg = e.what();
				}
				if (backup->total_cost() == 0) backup->poll(); // creates the list of files
				_cost_total += backup->total_cost();
			}
		}

		if (_state == FAILED)
		{
			_error = BACKUP_FAILED;
		} else
		{
			_state = BACKUP_FILES;
		}
		break;

	case BACKUP_NEXT:
		if (_cancelled)
		{
			_state = UNWIND_NEXT;
		} else
		{
			_state = BACKUP_FILES;
			FSObjectCopy *backup = _backups.front();
			_cost_to_backup += backup->total_cost();
			_backups_done.push(backup);
			_backups.pop_front();
			if (_backups.empty()) _state = DONE;
		}
		break;

	case BACKUP_FILES:
		{
			FSObjectCopy *backup = _backups.front();

			if (_cancelled)
			{
				_state = UNWIND_BACKUP;
				backup->start_unwind_move();
			} else
			{
				backup->poll();
				_cost_done = _cost_to_backup + backup->cost_done();
				if (backup->warning() != FSObjectCopy::NO_WARNING)
				{
					// Any warnings and it's not safe to delete either
					_can_cancel = false;
					_error = BACKUP_FAILED;
					_state = UNWIND_BACKUP;
					backup->start_unwind_move();
				} else 	if (backup->state() == FSObjectCopy::DONE)
				{
					if (backup->error())
					{
						_can_cancel = false;
						_error = BACKUP_FAILED;
						_errmsg = backup->error_msg();
						_state = UNWIND_BACKUP;
						backup->start_unwind_move();
					} else
					{
						_state = BACKUP_NEXT;
					}
				}
			}
		}
		break;

	case UNWIND_NEXT:
		if (_backups_done.empty())
		{
			_state = FAILED;
		} else
		{
			FSObjectCopy *backup = _backups_done.top();
			_backups.push_front(backup);
			_backups_done.pop();
			if (_backups_done.empty())
			{
				_cost_to_backup -= backup->total_cost();
			} else
			{
				_cost_to_backup = 0;
			}
		}
		break;

	case UNWIND_BACKUP:
		{
			FSObjectCopy *backup = _backups.front();
			backup->poll();
			_cost_done = _cost_to_backup + backup->unwind_cost();
			if (backup->state() == FSObjectCopy::DONE)
			{
				if (backup->warning())
				{
					_error = BACKUP_AND_UNWIND_FAILED;
				} else
				{
					// Delete parent BackupN directory if empty
					tbx::Path containing_dir = backup->target_dir();
					if (containing_dir.begin() == containing_dir.end())
					{
						containing_dir.remove();
					}
				}
				_state = UNWIND_NEXT;
			}
		}
		break;

	case FAILED:
		// Just reset the amount done to 0
		{
			std::string msg;
			if (_cancelled) msg = "You cancelled the backup";
			else msg = "Failed to create the backup";

			switch(_error)
			{
			case NO_ERROR: break; // Cancel pressed and no other error
			case BACKUP_FAILED: break; // Backup failed
			case BACKUP_AND_UNWIND_FAILED: msg += " and the unwind of the backup also failed"; break;
			}
			if (_error != NO_ERROR && !_errmsg.empty()) msg += "\n\n" + _errmsg;
			_cost_done = 0;
			_progress.value(0);
			close();

			tbx::show_message(msg, "PackMan backup", "error");
			more = false;
		}
		break;

	case DONE:
		_progress.value(1000);
		{
			BackupManager bum;

			while (!_backups_done.empty())
			{
				FSObjectCopy *backup = _backups_done.top();
				_backups_done.pop();
				bum.add(backup->target_path());
			}
			bum.commit();

			_run_command->execute(); // Execute the command

			close(); // This destroys this class
			more = false;
		}
		break;
	}

	return more;
}

/**
 * Cancel button has been clicked so stop move and unwind it.
 */
void BackupAndRun::cancel()
{
	if (_can_cancel) _cancelled = true;
}

/**
 * Faster button clicked so toggle between running faster and slower
 *
 * When running faster the whole desktop multitasking will suffer
 */
void BackupAndRun::faster()
{
	if (_run_faster)
	{
		_run_faster = false;
		_faster.text("Faster");
	} else
	{
		_run_faster = true;
		_faster.text("Slower");
	}
}

/**
 * Close down the window when move has finished
 */
void BackupAndRun::close()
{
	tbx::app()->remove_idle_command(this);
	_window.hide();
	_window.delete_object();
	delete this;
}

