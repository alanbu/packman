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

#include "tbx/application.h"
#include "tbx/messagewindow.h"
#include "MoveWindow.h"


MoveWindow::MoveWindow(const std::string &logical_path, const tbx::Path &app_path, const std::string &to_path) :
  _window("Move"),
  _do_cancel(this, &MoveWindow::cancel),
  _move_app(logical_path, app_path, to_path),
  _last_state(MoveApp::START)
{
	_status_text = _window.gadget(1);
	_progress = _window.gadget(2);
	_cancel = _window.gadget(3);
	_cancel.add_selected_command(&_do_cancel);
	_can_cancel = !_move_app.can_cancel(); // So first poll sets up cancel button

	_window.show();
	
	tbx::app()->add_idle_command(this);
}

/**
 * Routine polled by application on idle events
 */
void MoveWindow::execute()
{
	_move_app.poll();

	_progress.value(_move_app.decipercent_done());

	if (_move_app.state() != _last_state)
	{
		if (_can_cancel != _move_app.can_cancel())
		{
			_cancel.fade(_can_cancel);
			_can_cancel = _move_app.can_cancel();
			_cancel.text(_can_cancel ? "Cancel" : "Processing...");
		}

		_last_state = _move_app.state();
		switch (_move_app.state())
		{
		case MoveApp::BACKUP_FILES:
			_status_text.text("Backing up files");
			break;

		case MoveApp::COPYING_FILES:
			_status_text.text("Copying files");
			break;

		case MoveApp::UPDATE_PATHS:
			_status_text.text("Updating paths file");
			break;

		case MoveApp::UPDATE_VARS:
			_status_text.text("Updating system variables");
			break;

		case MoveApp::DELETE_OLD_FILES:
			_status_text.text("Deleting original files");
			break;

		case MoveApp::DONE:
			if (_move_app.warning()) show_warning();
			close();
			break;

		case MoveApp::UNWIND_COPY:
			if (_move_app.cancelled())
			{
				_status_text.text("Unwinding after cancel - deleting copied files");
				_progress.bar_colour(tbx::WimpColour::orange);
			} else
			{
				_status_text.text("Unwinding after error - deleting copied files");
				_progress.bar_colour(tbx::WimpColour::red);
			}
			break;

		case MoveApp::UNWIND_BACKUP:
			if (_move_app.cancelled())
			{
				_status_text.text("Unwinding after cancel - restoring from backup");
				_progress.bar_colour(tbx::WimpColour::orange);
			} else
			{
				_status_text.text("Unwinding after error - restoring from backup");
				_progress.bar_colour(tbx::WimpColour::red);
			}
			break;

		case MoveApp::FAILED:
			show_error();
			close();
			break;

		case MoveApp::START:
		case MoveApp::START_BACKUP:
			// Nothing required for these two steps.
			break;
		}
	}
}

/**
 * Cancel button has been clicked so stop move and unwind it.
 */
void MoveWindow::cancel()
{
	// MoveApp checks if the cancel is valid and does nothing
	// it it isn't
	_move_app.cancel();
}

/**
 * Close down the window when move has finished
 */
void MoveWindow::close()
{
	tbx::app()->remove_idle_command(this);
	_window.hide();
	_window.delete_object();
	delete this;
}

void MoveWindow::show_warning()
{
	std::string msg("Warning: ");
	msg += warning_text();
	tbx::show_message(msg, "Warning while moving application in PackMan","warning");
}

void MoveWindow::show_error()
{
	std::string msg("Error move failed: ");
	msg += error_text();
	if (_move_app.warning())
	{
		msg += "\n\nWarning: ";
		msg += warning_text();
	}
	tbx::show_message(msg, "Error moving Application in PackMan", "error");
}

std::string MoveWindow::warning_text()
{
	std::string text;
	switch(_move_app.warning())
	{
	case MoveApp::NO_WARNING: text = "No warning"; break;
	case MoveApp::DELETE_FAILED: text = "Delete failed"; break;
	case MoveApp::UNWIND_PATHS_FAILED: text = "Failed to undo changes to paths file"; break;
	case MoveApp::UNWIND_COPY_FAILED: text = "Failed to delete one or more files while undoing the copy"; break;
	case MoveApp::UNWIND_BACKUP_FAILED: text = "Failed to delete one or more files while undoing the backup"; break;
	}
	return text;
}

std::string MoveWindow::error_text()
{
	std::string text;
	switch(_move_app.error())
	{
	case MoveApp::NO_ERROR: text = "No error"; break;
	case MoveApp::COPY_FAILED: text = "Failed to copy a file"; break;
	case MoveApp::PATH_UPDATE_FAILED: text = "Failed to update the paths file"; break;
	case MoveApp::BACKUP_FAILED: text = "Failed to make a backup"; break;
	case MoveApp::CANCELLED: text = "The move was cancelled"; break;
	}

	return text;
}
