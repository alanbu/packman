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
/*
 * MovePath.h
 *
 *  Created on: 16 Apr 2012
 *      Author: alanb
 */

#ifndef MOVEPATH_H_
#define MOVEPATH_H_

#include "FSObjectCopy.h"
#include "ComponentsMoved.h"

#include "libpkg/path_table.h"
#include "tbx/path.h"

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
	enum State {FIND_INSTALLED, BUILD_FILE_LIST, CHECKING_FILES, COPYING_FILES, UPDATE_PATHS, UPDATE_VARS, UPDATE_BOOT_OPTIONS, DELETE_OLD_FILES, DONE, UNWIND_COPY, FAILED};
	enum Error {NO_ERROR, TARGET_FILE_EXISTS, NO_PATH_FOR_FILE, CANCELLED, CREATE_DIR_FAILED, COPY_FAILED, PATH_UPDATE_FAILED};
	enum Warning {NO_WARNING, DELETE_FAILED, UNWIND_COPY_FAILED};

private:
	std::string _path_name;
	std::string _new_dir;
	pkg::path_table _target_paths;
	State _state;
	Error _error;
	Warning _warning;
	ComponentsMoved *_comps_moved;
	std::queue<std::string> _installed;
	std::queue<std::pair<std::string, std::string> > _check_list;
	std::queue< std::pair<std::string, std::string> > _file_list;
	std::stack< std::pair<std::string, std::string> > _files_copied;
	std::set<tbx::Path> _dirs_created;
	tbx::Path _last_dir;
	long long _cost_done;
	long long _cost_total;
	int _cost_centipc;
	int _cost_one_item;
	bool _can_cancel;
	bool _cancelled;
	int _check_per_poll;

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

	std::string failed_target() const;

private:
	void build_installed_packages();
	void start_unwind_copy();
	bool add_files(const std::string &package);
	bool check_file(const std::string &package, const std::string &path_name);
	bool create_dir(tbx::Path dir);
};

#endif /* MOVEPATH_H_ */
