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

#include "tbx/application.h"
#include "tbx/messagewindow.h"
#include "tbx/hourglass.h"
#include "MoveWindow.h"
#include "PackManState.h"

// Number of times to poll MovePath from one WIMP poll when running faster
const int FASTER_LOOP_COUNT = 20;

/**
 * Construct window to move an applications
 *
 * @param logical_path logical path of application
 * @param app_path current location of application
 * @param to_path new location for application
 */
MoveWindow::MoveWindow(const std::string &logical_path, const tbx::Path &app_path, const std::string &to_path) :
  _window("Move"),
  _do_cancel(this, &MoveWindow::cancel),
  _do_faster(this, &MoveWindow::faster),
  _move_app(logical_path, app_path, to_path),
  _last_state(MoveApp::START)
{
	_status_text = _window.gadget(1);
	_progress = _window.gadget(2);
	_cancel = _window.gadget(3);
	_cancel.add_selected_command(&_do_cancel);
	_can_cancel = !_move_app.can_cancel(); // So first poll sets up cancel button

	_faster = _window.gadget(4);
	_faster.add_selected_command(&_do_faster);
	_run_faster = false;

	_window.show();
	
	pmstate()->moving(true);

	tbx::app()->add_idle_command(this);
}

/**
 * Routine polled by application on idle events
 */
void MoveWindow::execute()
{
	_move_app.poll();

	if (_run_faster && _last_state != MoveApp::DONE && _last_state != MoveApp::FAILED)
	{
		tbx::Hourglass hg;
		int j = FASTER_LOOP_COUNT;
		while (j-- && _move_app.state() == _last_state)
		{
			_move_app.poll();
		}
	}

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

		case MoveApp::UPDATE_BOOT_OPTIONS:
			_status_text.text("Updating boot options");
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
 * Faster button clicked so toggle between running faster and slower
 *
 * When running faster the whole desktop multitasking will suffer
 */
void MoveWindow::faster()
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
void MoveWindow::close()
{
	tbx::app()->remove_idle_command(this);
	_window.hide();
	_window.delete_object();
	delete this;
	pmstate()->moving(false);
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
