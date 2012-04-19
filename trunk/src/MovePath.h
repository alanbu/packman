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
 * MovePath.h
 *
 *  Created on: 16 Apr 2012
 *      Author: alanb
 */

#ifndef MOVEPATH_H_
#define MOVEPATH_H_

#include "FSObjectCopy.h"

#include "libpkg/path_table.h"
#include "tbx/Path.h"

#include <queue>
#include <vector>
#include <set>
#include <string>

/**
 * Class to update the path table with a new path location
 * and move any files installed at the previous path
 * to the new path.
 *
 * To use:
 *    Construct with path name and new path.
 *    Repeatedly call poll() until status() is DONE or FAILED;
 *    If an error occurs FAILED is returned at the end and error()
 *    is set with the reason
 *    If it failed and couldn't restore things to the original state or
 *    could not delete the original files after the move a warning is
 *    return in warning()
 */
class MovePath
{
public:
	enum State {FIND_INSTALLED, BUILD_FILE_LIST, COPYING_FILES, UPDATE_PATHS, DELETE_OLD_FILES, DONE, UNWIND_COPY, FAILED};
	enum Error {NO_ERROR, TARGET_FILE_EXISTS, CANCELLED, CREATE_DIR_FAILED, COPY_FAILED, PATH_UPDATE_FAILED};
	enum Warning {NO_WARNING, DELETE_FAILED, UNWIND_COPY_FAILED};

private:
	std::string _path_name;
	std::string _new_dir;
	pkg::path_table _target_paths;
	State _state;
	Error _error;
	Warning _warning;
	std::queue<std::string> _installed;
	std::queue< std::pair<std::string, std::string> > _file_list;
	std::stack< std::pair<std::string, std::string> > _files_copied;
	std::set<tbx::Path> _dirs_created;
	tbx::Path _last_dir;
	long long _cost_done;
	long long _cost_total;
	int _cost_centipc;
	int _cost_add_file;
	bool _can_cancel;
	bool _cancelled;

public:
	MovePath(const std::string &path_name, const std::string &new_dir);
	virtual ~MovePath();

	void poll();

	State state() const {return _state;}
	Error error() const {return _error;}
	Warning warning() const {return _warning;}
	int centipercent_done() const {return _cost_centipc;}

	bool can_cancel() const {return _can_cancel;}
	bool cancelled() const {return _cancelled;}
	void cancel();

private:
	void build_installed_packages();
	bool add_files(const std::string &package);
	bool create_dir(tbx::Path dir);
};

#endif /* MOVEPATH_H_ */
