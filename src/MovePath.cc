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
 * MovePath.cc
 *
 *  Created on: 16 Apr 2012
 *      Author: alanb
 */

#include "MovePath.h"
#include "Packages.h"
#include "libpkg/pkgbase.h"
#include "libpkg/sysvars.h"
#include <fstream>
#include <set>
#include <utility>

const unsigned int FILEOP_COST=16384;

// Percentages for each part of the progress in decipercent
const int INSTALLED_COST       = 10;
const int TOTAL_ADD_FILES_COST = 990;
const int TOTAL_CHECK_FILES_COST = 2000;
const int TOTAL_COPY_COST      = 6400;
const int UPDATE_PATHS_COST    = 200;
const int UPDATE_VARS_COST     = 400;

// Following constant speeds it up a little by doing multiple checks in a single
// poll - don't make it to large or the desktop responsiveness suffers
const int MIN_CHECK_PER_POLL = 2;
const int MAX_CHECK_PER_POLL = 10;

MovePath::MovePath(const std::string &path_name, const std::string &new_dir) :
  _path_name(path_name),
  _new_dir(new_dir),
  _state(FIND_INSTALLED),
  _error(NO_ERROR),
  _warning(NO_WARNING),
  _cost_done(0),
  _cost_total(0),
  _cost_centipc(0),
  _cost_one_item(0),
  _can_cancel(true),
  _cancelled(false),
  _check_per_poll(1)
{
	// Copy current path tables to temporary one used to resolve
	// the new file locations
	pkg::path_table &paths = Packages::instance()->package_base()->paths();
	for(pkg::path_table::const_iterator p = paths.begin(); p != paths.end();++p)
	{
		_target_paths.alter(p->first, p->second);
	}

	// Set changed path
	_target_paths.alter(path_name, new_dir);
}

MovePath::~MovePath()
{
}

/**
 * Do the move operation a piece at a time
 */
void MovePath::poll()
{
	switch(_state)
	{
	case FIND_INSTALLED:
		build_installed_packages();
		_state = BUILD_FILE_LIST;
		_cost_centipc =  INSTALLED_COST; // .1% of time on build installed packages
		if (!_installed.empty())
		{
			_cost_one_item = TOTAL_ADD_FILES_COST / _installed.size();
		}
		if (_cost_one_item < 1) _cost_one_item = 1;
		_check_list.push(std::make_pair("", _path_name));
		_cost_total += FILEOP_COST + FILEOP_COST;
		break;

	case BUILD_FILE_LIST:
		if (_installed.empty())
		{
			_state = CHECKING_FILES;
			_cost_centipc = INSTALLED_COST + TOTAL_ADD_FILES_COST; // In case of rounding errors.
			_last_dir = tbx::Path("");
			_check_per_poll = (_check_list.size() * 10) / TOTAL_CHECK_FILES_COST;
			if (_check_per_poll < MIN_CHECK_PER_POLL) _check_per_poll = MIN_CHECK_PER_POLL;
			if (_check_per_poll > MAX_CHECK_PER_POLL) _check_per_poll = MAX_CHECK_PER_POLL;
		} else
		{
			std::string package = _installed.front();
			if (add_files(package))
			{
				_installed.pop();
				_cost_centipc += _cost_one_item;
			} else
			{
				_state = FAILED;
			}
		}
		break;

	case CHECKING_FILES:
		if (_cancelled)
		{
			_state = FAILED;
		} else if (_check_list.empty())
		{
			_last_dir = tbx::Path("");
			_state = COPYING_FILES;
			_cost_centipc = INSTALLED_COST + TOTAL_ADD_FILES_COST + TOTAL_CHECK_FILES_COST;
		} else
		{
			// Do multiple checks in one poll for speed
			int j = _check_per_poll;
			while (j-- && !_check_list.empty() && _state == CHECKING_FILES)
			{
				std::pair<std::string, std::string> check = _check_list.front();
				if (check_file(check.first, check.second))
				{
					_check_list.pop();
					int done = _file_list.size();
					_cost_centipc =  INSTALLED_COST + TOTAL_ADD_FILES_COST
							+ ((done * TOTAL_CHECK_FILES_COST)/ (done + _check_list.size()));
				} else
				{
					_state = FAILED;
				}
			}
		}
		break;

	case COPYING_FILES:
		if (_cancelled)
		{
			start_unwind_copy();
		} else if (_file_list.empty())
		{
			_state = UPDATE_PATHS;
		} else
		{
			std::pair<std::string, std::string> files = _file_list.front();
			tbx::Path source(files.first);
		    tbx::Path target(files.second);
			tbx::PathInfo info;

			if (source.path_info(info))
			{
				if (info.directory())
				{
					if (create_dir(target))
					{
						_cost_done += FILEOP_COST;
					} else
					{
						_error = CREATE_DIR_FAILED;
					}
				} else if (source.copy(target))
				{
					_cost_done += FILEOP_COST + info.length();
				}
				else
				{
					_error = COPY_FAILED;
				}
				if (_error == 0)
				{
					_files_copied.push(files);
					_file_list.pop();

					if (_cost_total)
					{
						_cost_centipc = INSTALLED_COST + TOTAL_ADD_FILES_COST + TOTAL_CHECK_FILES_COST
								+ (int)(((long long)TOTAL_COPY_COST * _cost_done)/_cost_total);
					}
				} else
				{
					start_unwind_copy();
				}
			}
		}
		break;

	case UPDATE_PATHS:
		_can_cancel = false;
		if (_cancelled)
		{
			start_unwind_copy();
		} else
		{
			pkg::path_table &paths = Packages::instance()->package_base()->paths();
			try
			{
				std::string path_defn = Packages::make_path_definition(_new_dir);

				paths.alter(_path_name, path_defn);
				paths.commit();
				_state = UPDATE_VARS;
				_cost_centipc += UPDATE_PATHS_COST;
			} catch(...)
			{
				start_unwind_copy();
				_error = PATH_UPDATE_FAILED;
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
		_cost_centipc += UPDATE_VARS_COST;
		_last_dir = tbx::Path("");
		break;


	case DELETE_OLD_FILES:
		if (_files_copied.empty())
		{
			_state = DONE;
			_cost_centipc = 10000;
		} else
		{
			std::pair<std::string, std::string> files = _files_copied.top();
			tbx::Path source(files.first);

			// Safe to just call remove as it will not delete non-empty dirs
			source.remove();
			_files_copied.pop();

			_cost_done += FILEOP_COST;
			if (_cost_total)
			{
				_cost_centipc = INSTALLED_COST + TOTAL_ADD_FILES_COST + TOTAL_CHECK_FILES_COST + UPDATE_PATHS_COST + UPDATE_VARS_COST
						+ (int)(((long long)TOTAL_COPY_COST * _cost_done)/_cost_total);
			}
		}
		break;

	case UNWIND_COPY:
		if (_files_copied.empty())
		{
			_state = FAILED;
			_cost_centipc = 0;
		} else
		{
			std::pair<std::string, std::string> files = _files_copied.top();
			tbx::Path target(files.second);

			if (target.directory())
			{
				if (_dirs_created.find(target.name()) != _dirs_created.end())
				{
					target.remove();
				}
			} else if (!target.remove())
			{
				if (_warning == NO_WARNING) _warning = UNWIND_COPY_FAILED;
			}
			_files_copied.pop();
			_cost_done -= _cost_one_item;
			_cost_centipc = _cost_done / _cost_total;
		}
		break;

	case FAILED:
		// Just reset the amount done to 0 - caller should stop the poll now
		_cost_centipc = 0;
		break;
	case DONE:
		// Nothing to do - caller should stop the poll now
		break;
	}
}

/**
 * Get list of installed packages
 */
void MovePath::build_installed_packages()
{
   pkg::pkgbase *package_base = Packages::instance()->package_base();
   const pkg::binary_control_table& ctrltab = package_base->control();
   std::string prev_pkgname;

   for (pkg::binary_control_table::const_iterator i=ctrltab.begin();
	 i !=ctrltab.end(); ++i)
   {
	  std::string pkgname=i->first.pkgname;
	  if (pkgname!=prev_pkgname)
	  {
		  prev_pkgname=pkgname;

		  pkg::status_table::const_iterator sti = package_base->curstat().find(pkgname);

		  if (sti != package_base->curstat().end()
			  && (*sti).second.state() == pkg::status::state_installed)
		  {
			  _installed.push(pkgname);
		  }
	  }
   }
}

/**
 * Add files from package that will need to be moved
 *
 * @param package name of package to add files from
 * @param return true
 */
bool MovePath::add_files(const std::string &package)
{
    pkg::pkgbase *package_base = Packages::instance()->package_base();
	std::string prefix=package_base->info_pathname(package);
	std::string files_pathname=prefix + ".Files";
	std::string files_bak_pathname=prefix + ".Files--";
	std::set<std::string> mf;

    unsigned int path_name_length = _path_name.length();

	// Get files from manifest that match or are children of the logical path
	std::ifstream dst_in(files_pathname.c_str());
	dst_in.peek();
	while (dst_in&&!dst_in.eof())
	{
		std::string line;
		std::getline(dst_in,line);
		if (line.length() >= path_name_length
			&& _path_name.compare(0, path_name_length, line, 0, path_name_length) == 0
			)
		{
			// Rough comparison only - extra checks are done below
			mf.insert(line);
		}
		dst_in.peek();
	}

	// Add in from backup file if it exists
	std::ifstream bak_in(files_bak_pathname.c_str());
	bak_in.peek();
	while (bak_in&&!bak_in.eof())
	{
		std::string line;
		std::getline(bak_in,line);
		if (line.length() >= path_name_length
			&& _path_name.compare(0, path_name_length, line) == 0
			)
		{
			// Rough comparison only - extra checks are done below
			mf.insert(line);
		}
		bak_in.peek();
	}

	// Figure out which files to copy
	std::set<std::string> dirs; // List of directories needed

	// Ensure base directory is created if necessary
	dirs.insert(_path_name);

	std::string last_dir = _path_name;

    for (std::set<std::string>::iterator i = mf.begin(); i != mf.end(); ++i)
    {
    	std::string mf_file = *i;
    	std::string::size_type leaf_pos = mf_file.find_last_of('.');
    	if (leaf_pos != std::string::npos)
    	{
    		// Ensure new directories are added to the list
    		std::string dir = mf_file.substr(0, leaf_pos);
    		if (dir != last_dir)
    		{
    			last_dir = dir;

				std::stack< std::string > newdirs;
				while (dirs.find(dir) == dirs.end())
				{
					dirs.insert(dir);
					newdirs.push(dir);
			    	leaf_pos = dir.find_last_of('.');
			    	if (leaf_pos != std::string::npos) dir.erase(leaf_pos);
				}

				while (!newdirs.empty())
				{
        			_check_list.push(std::make_pair(package, newdirs.top()));
					newdirs.pop();
				}
    		}
    	}

    	_check_list.push(std::make_pair(package, mf_file));
    }

    return true;
}


/**
 * Check source and target path and create list of files to be copied.
 *
 * Checks:
 *   source path exists
 *   target path doesn't
 *   path is not a sub path that goes to a different location
 *
 * @param package for file
 * @param path_name logical path name for file
 * @returns true if file is OK or does not need to be copied, false on error
 */
bool MovePath::check_file(const std::string &package, const std::string &path_name)
{
	pkg::path_table &paths = Packages::instance()->package_base()->paths();
	std::string old_loc = paths(path_name, package);
	std::string new_loc = _target_paths(path_name, package);

	if (old_loc != new_loc)
	{
		tbx::Path old_path(old_loc);
		tbx::PathInfo info;
		if (old_path.path_info(info))
		{
			_cost_total += FILEOP_COST + FILEOP_COST;

			if (!info.directory())
			{
				_cost_total += info.length();

				tbx::Path new_path(new_loc);

				if (new_path.exists())
				{
					_error = TARGET_FILE_EXISTS;
					return false;
				}
			}

    		_file_list.push(std::make_pair(old_loc, new_loc));
		}
	}

	return true;
}

/**
 * Create directory (and its parents) if necessary and add it to list
 * for rollback.
 *
 * @param dir directory to create
 * @returns true if directory was created or already exists
 */
bool MovePath::create_dir(tbx::Path dir)
{
	tbx::PathInfo info;
	if (dir.path_info(info))
	{
		return info.directory();
	}

	tbx::Path parent(dir.parent());
	if (create_dir(parent))
	{
		if (dir.create_directory())
		{
			_dirs_created.insert(dir);
			return true;
		}
	}

	return false;
}

/**
 * Start unwind of copy
 */
void MovePath::start_unwind_copy()
{
	if (_files_copied.empty())
	{
		_state = FAILED;
		_cost_centipc = 0;
	} else
	{
		_state = UNWIND_COPY;
		 _cost_done = _cost_centipc * 1000;
		 _cost_total = 1000;
		_cost_one_item = _cost_done / _files_copied.size();
		if (_cost_one_item < 1)
		{
			_cost_one_item = 1;
			_cost_done = _files_copied.size();
			_cost_total = (_cost_done / 100) + 1;
		}
	}
}


/**
 * Stop the move (if possible)
 *
 * Once cancelled the state will change to unwind everything done
 * on next poll.
 */
void MovePath::cancel()
{
	if (_can_cancel)
	{
		_can_cancel = false; // Can only do it once
		_cancelled = true;
		if (_error == NO_ERROR) _error = CANCELLED;
	}
}

/**
 * Get name of target file when an error occured
 */
std::string MovePath::failed_target() const
{
	std::string result;
	switch(_error)
	{
	case TARGET_FILE_EXISTS:
		result = _check_list.front().second;
		break;
	case COPY_FAILED:
	case CREATE_DIR_FAILED:
	    result = _file_list.front().second;
		break;
	default:
		// Nothing for other error codes
		break;
	}

	return result;
}
