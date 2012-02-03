
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
  _state(START),
  _error(NO_ERROR),
  _warning(NO_WARNING),
  _cost_done(0),
  _cost_total(0),
  _cost_stage_start(0)
{
	_copy_handler.delete_option(FSObjectCopy::DELETE_ON_SECOND_PASS);
}

void MoveApp::poll()
{
	switch(_state)
	{
	case START:
		_state = COPYING_FILES;
		_cost_stage_start = 0;
		break;

	case COPYING_FILES:
		{
			_copy_handler.poll();
			if (_cost_total == 0) calc_cost_total();
			_cost_done = _cost_stage_start + _copy_handler.cost_done();

			if (_copy_handler.state() == FSObjectCopy::DONE)
			{
				if (_copy_handler.error())
				{
					_error = COPY_FAILED;
					_state = FAILED;
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
 */
void MoveApp::calc_cost_total()
{
	_cost_total = _copy_handler.total_cost();
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
