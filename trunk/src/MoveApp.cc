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

#include "MoveApp.h"
#include "Packages.h"
#include "BackupManager.h"
#include "libpkg/pkgbase.h"
#include "libpkg/sysvars.h"
#include "libpkg/path_table.h"

/* Rough costs of timings for each operation specified as
 * the approximate number of bytes that could be copied
 * in the same time.
 */
const int PATHS_COST  = 50000;
const int VARS_COST   = 100000;

MoveApp::MoveApp(const std::string &logical_path, const std::string &app_path, const std::string &to_path) :
  _logical_path(logical_path),
  _copy_handler(app_path, tbx::Path(to_path).parent()),
  _backup_handler(0),
  _state(START),
  _error(NO_ERROR),
  _warning(NO_WARNING),
  _cost_done(0),
  _cost_total(0),
  _cost_stage_start(0),
  _can_cancel(true),
  _cancelled(false)
{
	_copy_handler.delete_option(FSObjectCopy::DELETE_ON_SECOND_PASS);
	tbx::Path check_target(to_path);
	if (check_target.exists()) setup_backup(check_target);
}

/**
 * Setup backup
 *
 * @param file/directory to backup
 */
void MoveApp::setup_backup(const tbx::Path &target)
{
	// Check target exists so we need to make a backup
	tbx::Path backup_dir = BackupManager::get_backup_dir(target);

	_backup_handler = new FSObjectCopy(target, backup_dir);
	_backup_handler->delete_option(FSObjectCopy::DELETE_AFTER_COPY);
	_state = START_BACKUP;
}

/**
 * Destructor - just needs to delete backup handler if used
 */
MoveApp::~MoveApp()
{
	delete _backup_handler;
}

/**
 * Do the next stage of the move
 *
 * e.g. copy files, update paths text
 *
 * move is finished when stage() == DONE or FAILED
 */
void MoveApp::poll()
{
	switch(_state)
	{
	case START_BACKUP:
		if (_backup_handler)
		{
			tbx::Path backup_dir(_backup_handler->target_dir());
			if (backup_dir.create_directory())
			{
				_state = BACKUP_FILES;
			}
		}
		if (_state != BACKUP_FILES)
		{
			_error = BACKUP_FAILED;
			_state = FAILED;
		}
		break;

	case BACKUP_FILES:
		if (_cancelled)
		{
			_state = UNWIND_BACKUP;
			_backup_handler->start_unwind_move();
		} else
		{
			_backup_handler->poll();
			if (_cost_total == 0) calc_cost_total(true);
			_cost_done = _backup_handler->cost_done();
			if (_backup_handler->warning() != FSObjectCopy::NO_WARNING)
			{
				// Any warnings and it's not safe to delete either
				_can_cancel = false;
				_error = BACKUP_FAILED;
				_state = UNWIND_BACKUP;
				_backup_handler->start_unwind_move();
			} else 	if (_backup_handler->state() == FSObjectCopy::DONE)
			{
				if (_backup_handler->error())
				{
					_can_cancel = false;
					_error = BACKUP_FAILED;
					_state = UNWIND_BACKUP;
					_backup_handler->start_unwind_move();
				} else
				{
					_state = START;
				}
			}
		}
		break;

	case START:
		_state = COPYING_FILES;
		_cost_stage_start = _cost_done;
		_cost_total = 0; // Cost will need to be recalculated if a backup was done
		break;

	case COPYING_FILES:
		if (_cancelled)
		{
			_state = UNWIND_COPY;
			_copy_handler.start_unwind_copy();
		} else
		{
			_copy_handler.poll();
			if (_cost_total == 0) calc_cost_total(false);
			_cost_done = _cost_stage_start + _copy_handler.cost_done();

			if (_copy_handler.state() == FSObjectCopy::DONE)
			{
				if (_copy_handler.error())
				{
					_can_cancel = false;
					_error = COPY_FAILED;
					if (_backup_handler)
					{
						_state = UNWIND_BACKUP;
						_backup_handler->start_unwind_move();
					}
					else _state = FAILED;
				} else
				{
					_state = UPDATE_PATHS;
				}
			} else if (_copy_handler.error())
			{
				// Change state so UI knows its unwinding
				_can_cancel = false;
				_error = COPY_FAILED;
				_state = UNWIND_COPY;
			}
		}
		break;

	case UPDATE_PATHS:
		if (_cancelled)
		{
			_state = UNWIND_COPY;
			_copy_handler.start_unwind_copy();
		} else
		{
			pkg::path_table &paths = Packages::instance()->package_base()->paths();
			try
			{
				std::string path_defn = Packages::make_path_definition(_copy_handler.target_path());
				paths.alter(_logical_path, path_defn);
				paths.commit();
				_state = UPDATE_VARS;
				_cost_done += PATHS_COST;
			} catch(...)
			{
				_state = UNWIND_COPY;
				_error = PATH_UPDATE_FAILED;
				_copy_handler.start_unwind_copy();
			}
		}
		break;

	case UPDATE_VARS:
		 // From here on in it's tidying up so you can't cancel
		_can_cancel = false;

		// Note: For simplicity update vars is assumed to never fail
		// if it ever did it would be corrected by the next install
		// or update.
		pkg::update_sysvars(*(Packages::instance()->package_base()));
		_state = DELETE_OLD_FILES;
		_cost_done += VARS_COST;
		_copy_handler.start_delete_source();
		_cost_stage_start = _cost_done - _copy_handler.cost_done();
		break;

	case DELETE_OLD_FILES:
		{
			// Note: If old files fail to delete we will just issue
			// a warning as the new version will now be used in
			// preference.
			_copy_handler.poll();
			_cost_done = _cost_stage_start + _copy_handler.cost_done();
			if (_copy_handler.state() == FSObjectCopy::DONE)
			{
			   if (_copy_handler.warning()) _warning = DELETE_FAILED;
			   _state = DONE;
				if (_backup_handler)
				{
					// Add backup to backup manager
					BackupManager bm;
					bm.add(_backup_handler->target_path());
					bm.commit();
				}
			}
		}
		break;

	case UNWIND_COPY:
		_copy_handler.poll();
		_cost_done = _copy_handler.unwind_cost();
		if (_backup_handler) _cost_done += _backup_handler->unwind_cost();
		if (_copy_handler.state() == FSObjectCopy::DONE)
		{
			if (_warning == NO_WARNING && _copy_handler.warning()) _warning = UNWIND_COPY_FAILED;
			if (_backup_handler)
			{
				_state = UNWIND_BACKUP;
				_backup_handler->start_unwind_move();
			}
			else _state = FAILED;
		}
		break;

	case UNWIND_BACKUP:
		_backup_handler->poll();
		_cost_done = _backup_handler->unwind_cost();
		if (_backup_handler->state() == FSObjectCopy::DONE)
		{
			if (_warning == NO_WARNING && _backup_handler->warning())
			{
				_warning = UNWIND_BACKUP_FAILED;
			} else
			{
				// Delete parent BackupN directory if empty
				tbx::Path containing_dir = _backup_handler->target_dir();
				if (containing_dir.begin() == containing_dir.end())
				{
					containing_dir.remove();
				}
			}
			_state = FAILED;
		}
		break;

	case FAILED:
		// Just reset the amount done to 0 - caller should stop the poll now
		_cost_done = 0;
		break;
	case DONE:
		// Nothing to do - caller should stop the poll now
		break;
	}
}

/**
 * Stop the move (if possible)
 *
 * Once cancelled the state will change to unwind everything done
 * on next poll.
 */
void MoveApp::cancel()
{
	if (_can_cancel)
	{
		_can_cancel = false; // Can only do it once
		_cancelled = true;
		if (_error == NO_ERROR) _error = CANCELLED;
	}
}

/**
 * Calculate total cost to do the move.
 *
 * Value calculated is equivalent to approximate number of
 * bytes copied, with non-copy operations using a calculated
 * default.
 *
 * @param backup_only true if we can only use the backup
 * cost to estimate the cost.
 */
void MoveApp::calc_cost_total(bool backup_only)
{
	_cost_total = 0;
	if (_backup_handler)
	{
		_cost_total = _backup_handler->total_cost();
	}
	if (backup_only) _cost_total <<=1; // Times backup cost by 2 as an estimate
	else _cost_total += _copy_handler.total_cost();
	_cost_total += PATHS_COST;
	_cost_total += VARS_COST;
}

/**
 * Get the percentage of the move that has been done * 10
 */
int MoveApp::decipercent_done() const
{
	return (int)((_cost_total == 0) ? 0 : ((_cost_done * 1000) / _cost_total));
}
