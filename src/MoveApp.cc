
#include "MoveApp.h"
#include "Packages.h"
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
  _copy_handler(app_path, to_path),
  _backup_handler(0),
  _state(START),
  _error(NO_ERROR),
  _warning(NO_WARNING),
  _cost_done(0),
  _cost_total(0),
  _cost_stage_start(0)
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
	tbx::Path backup_dir;
	char backup_dirname[20];
	int backup_dirnum = 0;
	do
	{
		backup_dirnum++;
		sprintf(backup_dirname, "Backup%d", backup_dirnum);
		backup_dir = target.parent();
		backup_dir.down(backup_dirname);
	} while (backup_dir.exists());

	backup_dir.down(target.leaf_name());
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
		_backup_handler->poll();
		if (_cost_total == 0) calc_cost_total(true);
		_cost_done = _backup_handler->cost_done();
		if (_backup_handler->state() == FSObjectCopy::DONE)
		{
			if (_backup_handler->error())
			{
				_error = BACKUP_FAILED;
				_state = FAILED;
			} else
			{
				_state = START;
			}
		}
		break;

	case START:
		_state = COPYING_FILES;
		_cost_stage_start = _cost_done;
		_cost_total = 0; // Cost will need to be recalculated if a backup was done
		break;

	case COPYING_FILES:
		{
			_copy_handler.poll();
			if (_cost_total == 0) calc_cost_total(false);
			_cost_done = _cost_stage_start + _copy_handler.cost_done();

			if (_copy_handler.state() == FSObjectCopy::DONE)
			{
				if (_copy_handler.error())
				{
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
			}
		}
		break;

	case UPDATE_PATHS:
		{
			pkg::path_table &paths = Packages::instance()->package_base()->paths();
			try
			{
				paths.alter(_logical_path, _copy_handler.target_path());
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
			}
		}
		break;

	case UNWIND_COPY:
		_copy_handler.poll();
		if (_copy_handler.state() == FSObjectCopy::DONE)
		{
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
		if (_backup_handler->state() == FSObjectCopy::DONE)
		{
			//TODO: Check unwind was successful
			_state = FAILED;
		}
		break;

	case DONE:
	case FAILED:
		// Nothing to do - caller should stop the poll now
		break;
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
