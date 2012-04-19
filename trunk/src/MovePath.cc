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
#include <fstream>
#include <set>
#include <utility>

const unsigned int FILEOP_COST=16384;

// Percentages for each part of the progress in decipercent
const int INSTALLED_COST       = 10;
const int TOTAL_ADD_FILES_COST = 990;
const int TOTAL_COPY_COST      = 8800;
const int UPDATE_PATHS_COST    = 200;

MovePath::MovePath(const std::string &path_name, const std::string &new_dir) :
  _path_name(path_name),
  _new_dir(new_dir),
  _state(FIND_INSTALLED),
  _error(NO_ERROR),
  _warning(NO_WARNING),
  _cost_done(0),
  _cost_total(0),
  _cost_centipc(0),
  _cost_add_file(0),
  _can_cancel(true),
  _cancelled(false)
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
		if (_installed.size()) _cost_add_file = TOTAL_ADD_FILES_COST / _installed.size();
		if (_cost_add_file < 1) _cost_add_file = 1;
		break;

	case BUILD_FILE_LIST:
		if (_installed.empty())
		{
			_state = COPYING_FILES;
			_cost_centipc = INSTALLED_COST + TOTAL_ADD_FILES_COST; // In case of rounding errors.
		} else
		{
			std::string package = _installed.front();
			if (add_files(package))
			{
				_installed.pop();
				_cost_centipc += _cost_add_file;
			} else
			{
				_state = FAILED;
			}
		}
		break;

	case COPYING_FILES:
		if (_cancelled)
		{
			_state = UNWIND_COPY;
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
						_cost_centipc = INSTALLED_COST + TOTAL_ADD_FILES_COST + (int)(((long long)TOTAL_COPY_COST * _cost_done)/_cost_total);
					}
				} else
				{
					_state = UNWIND_COPY;
				}
			}
		}
		break;

	case UPDATE_PATHS:
		_can_cancel = false;
		if (_cancelled)
		{
			_state = UNWIND_COPY;
		} else
		{
			pkg::path_table &paths = Packages::instance()->package_base()->paths();
			try
			{
				paths.alter(_path_name, _new_dir);
				paths.commit();
				_state = DELETE_OLD_FILES;
				_last_dir = tbx::Path("");
				_cost_centipc += UPDATE_PATHS_COST;
			} catch(...)
			{
				_state = UNWIND_COPY;
				_error = PATH_UPDATE_FAILED;
			}
		}
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

			_cost_total += FILEOP_COST;
			if (_cost_total)
			{
				_cost_centipc = INSTALLED_COST + TOTAL_ADD_FILES_COST + UPDATE_PATHS_COST
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

	// Get files manifest
	std::ifstream dst_in(files_pathname.c_str());
	dst_in.peek();
	while (dst_in&&!dst_in.eof())
	{
		std::string line;
		std::getline(dst_in,line);
		if (line.length()) mf.insert(line);
		dst_in.peek();
	}

	// Add in from backup file if it exists
	std::ifstream bak_in(files_bak_pathname.c_str());
	bak_in.peek();
	while (bak_in&&!bak_in.eof())
	{
		std::string line;
		std::getline(bak_in,line);
		if (line.length()) mf.insert(line);
		bak_in.peek();
	}

	// Figure out which files to copy
	pkg::path_table &paths = Packages::instance()->package_base()->paths();
	std::set<std::string> dirs; // List of directories needed

	std::string base_path = paths(_path_name, package);
	dirs.insert(base_path);

	tbx::Path last_dir(base_path);

    for (std::set<std::string>::iterator i = mf.begin(); i != mf.end(); ++i)
    {
    	std::string mf_file = *i;
    	std::string old_loc = paths(mf_file, package);
    	std::string new_loc = _target_paths(mf_file, package);

    	if (old_loc != new_loc)
    	{
    		tbx::Path old_path(old_loc);
    		tbx::PathInfo info;
    		if (old_path.path_info(info))
    		{
    			tbx::Path new_path(new_loc);

    			_cost_total += FILEOP_COST + FILEOP_COST + info.length();
        		if (new_path.exists())
        		{
        			_error = TARGET_FILE_EXISTS;
        			return false;
        		}

        		// Add directories into list
        		tbx::Path parent_dir(old_path.parent());
        		if (parent_dir != last_dir)
        		{
        			tbx::Path new_dir(new_path.parent());
					std::set<std::string>::iterator found_dir;
					while ((found_dir = dirs.find(parent_dir.name())) == dirs.end())
					{
						dirs.insert(parent_dir.name());
						_file_list.push(std::make_pair(parent_dir.name(), new_dir.name()));
						_cost_total += FILEOP_COST + FILEOP_COST;
						parent_dir.up();
						new_dir.up();
					}
        		}

        		_file_list.push(std::make_pair(old_loc, new_loc));
    		}
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
