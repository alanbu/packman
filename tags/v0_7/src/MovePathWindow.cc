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
#include "tbx/hourglass.h"
#include "MovePathWindow.h"
#include "PackManState.h"

// Number of times to poll MovePath from one WIMP poll when running faster
const int FASTER_LOOP_COUNT = 20;

/**
 * Construct the move path window
 *
 * @param logical_path logical path to be moved
 * @param to_path new location for the logical path
 */
MovePathWindow::MovePathWindow(const std::string &logical_path, const std::string &to_path) :
  _window("Move"), // Can share status window with MoveWindow
  _do_cancel(this, &MovePathWindow::cancel),
  _do_faster(this, &MovePathWindow::faster),
  _move_path(logical_path, to_path),
  _last_state(MovePath::FIND_INSTALLED)
{
	_status_text = _window.gadget(1);
	_status_text.text("Finding installed packages");
	_progress = _window.gadget(2);
	_progress.upper_bound(10000);

	_cancel = _window.gadget(3);
	_cancel.add_selected_command(&_do_cancel);
	_can_cancel = !_move_path.can_cancel(); // So first poll sets up cancel button

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
void MovePathWindow::execute()
{
	_move_path.poll();

	if (_run_faster && _last_state != MovePath::DONE && _last_state != MovePath::FAILED)
	{
		tbx::Hourglass hg;
		int j = FASTER_LOOP_COUNT;
		while (j-- && _move_path.state() == _last_state)
		{
			_move_path.poll();
		}
	}

	_progress.value(_move_path.centipercent_done());

	if (_move_path.state() != _last_state)
	{
		if (_can_cancel != _move_path.can_cancel())
		{
			_cancel.fade(_can_cancel);
			_can_cancel = _move_path.can_cancel();
			_cancel.text(_can_cancel ? "Cancel" : "Processing...");
		}

		_last_state = _move_path.state();
		switch (_move_path.state())
		{
		case MovePath::BUILD_FILE_LIST:
			_status_text.text("Building list of files to move");
			break;

		case MovePath::CHECKING_FILES:
			_status_text.text("Checking files");
			break;

		case MovePath::COPYING_FILES:
			_status_text.text("Copying files");
			break;

		case MovePath::UPDATE_PATHS:
			_status_text.text("Updating paths file");
			break;

		case MovePath::UPDATE_VARS:
			_status_text.text("Updating system variables");
			break;

		case MovePath::DELETE_OLD_FILES:
			_status_text.text("Deleting original files");
			break;

		case MovePath::DONE:
			if (_move_path.warning()) show_warning();
			close();
			break;

		case MovePath::UNWIND_COPY:
			if (_move_path.cancelled())
			{
				_status_text.text("Unwinding after cancel - deleting copied files");
				_progress.bar_colour(tbx::WimpColour::orange);
			} else
			{
				_status_text.text("Unwinding after error - deleting copied files");
				_progress.bar_colour(tbx::WimpColour::red);
			}
			break;

		case MovePath::FAILED:
			show_error();
			close();
			break;

		case MovePath::FIND_INSTALLED:
			// Nothing required for these steps.
			break;
		}
	}
}

/**
 * Cancel button has been clicked so stop move and unwind it.
 */
void MovePathWindow::cancel()
{
	// MovePath checks if the cancel is valid and does nothing
	// it it isn't
	_move_path.cancel();
}


/**
 * Faster button clicked so toggle between running faster and slower
 *
 * When running faster the whole desktop multitasking will suffer
 */
void MovePathWindow::faster()
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
void MovePathWindow::close()
{
	tbx::app()->remove_idle_command(this);
	_window.hide();
	_window.delete_object();
	delete this;
	pmstate()->moving(false);
}

void MovePathWindow::show_warning()
{
	std::string msg("Warning: ");
	msg += warning_text();
	tbx::show_message(msg, "Warning while moving a path in PackMan","warning");
}

void MovePathWindow::show_error()
{
	std::string msg("Error move failed: ");
	msg += error_text();
	std::string target_file = _move_path.failed_target();
	if (!target_file.empty())
	{
		msg += "\nTarget file:\n";
		msg += target_file;
	}
	if (_move_path.warning())
	{
		msg += "\n\nWarning: ";
		msg += warning_text();
	}
	tbx::show_message(msg, "Error moving a path in PackMan", "error");
}

std::string MovePathWindow::warning_text()
{
	std::string text;
	switch(_move_path.warning())
	{
	case MovePath::NO_WARNING: text = "No warning"; break;
	case MovePath::DELETE_FAILED: text = "Failed to delete old files after move"; break;
	case MovePath::UNWIND_COPY_FAILED: text = "Failed to delete all the files that had been copied to the new location"; break;
	}
	return text;
}

std::string MovePathWindow::error_text()
{
	std::string text;
	switch(_move_path.error())
	{
	case MovePath::NO_ERROR: text = "No error"; break;
	case MovePath::TARGET_FILE_EXISTS: text = "An installed file already exists at the destination"; break;
	case MovePath::CREATE_DIR_FAILED: text = "Unable to create a directory at the new location"; break;
	case MovePath::COPY_FAILED: text = "Failed to copy a file"; break;
	case MovePath::PATH_UPDATE_FAILED: text = "Failed to update the paths file"; break;
	case MovePath::CANCELLED: text = "The move was cancelled"; break;
	}

	return text;
}
