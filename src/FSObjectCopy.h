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

#ifndef FSOBJECTCOPY_H
#define FSOBJECTCOPY_H

#include <string>
#include <queue>
#include <stack>

#include "tbx/path.h"

/**
 * Class to copy a file or directory to another location
 * using polling.
 *
 * The target should not exist before the copy
 *
 * To use:
 *   Create an instance with the source and target
 *   repeatedly call poll until status() == DONE
 *   Check error() for errors and warning() for warnings
 *   If an error occurs an attempt is made to delete any
 *   files copied so far - check warning() to see if it
 *   was successful.
 */
class FSObjectCopy
{
public:
	enum State {BUILD_LIST, COPY_OBJECT, DELETE_SOURCE, UNWIND_COPY_OBJECT, UNWIND_MOVE_OBJECT, DONE};
	enum Error {NO_ERROR, LEAF_NAME_MISMATCH, TARGET_EXISTS, BUILD_LIST_FAILED, COPY_FAILED };
	enum Warning {NO_WARNING, DELETE_FAILED, DELETE_TARGET_FAILED, COPY_BACK_FAILED};
	enum DeleteOption {DO_NOT_DELETE, DELETE_AFTER_COPY, DELETE_ON_SECOND_PASS};

private:
	tbx::Path _source;
	tbx::Path _target_dir;
	tbx::Path _source_dir;
	DeleteOption _delete_option;
    State _state;
	Error _error;
	Warning _warning;
	std::queue<std::string> _to_copy;
	std::stack<std::string> _copied;
	std::stack<std::string> _deleted;
    long long _byte_total;
    int _file_total;
    int _dir_total;
    long long _byte_done;
    int _file_done;
    int _dir_done;
    int _del_done;
    int _unwind_done;

public:
	FSObjectCopy(const std::string &source, const std::string &target);

	tbx::Path target_path() const {return tbx::Path(_target_dir, _source.leaf_name());}
	const tbx::Path &target_dir() const {return _target_dir;}

	void poll();
	void start_delete_source();
	void start_unwind_copy();
	void start_unwind_move();

	/**
	 * Optionally set to delete files after the copy
	 */
	void delete_option(DeleteOption opt) {_delete_option = opt;}
	DeleteOption delete_option() const {return _delete_option;}

	State state() {return _state;}
	Error error() {return _error;}
	Warning warning() {return _warning;}

	long long byte_total() {return _byte_total;}
	int file_total() {return _file_total;}
	int dir_total() {return _dir_total;}
	long long byte_done() {return _byte_done;}
	int file_done() {return _file_done;}
	int dir_done() {return _dir_done;}

	long long total_cost();
	long long cost_done();
	long long delete_source_cost();
	int scaled_done();
	long long unwind_cost();

private:
	bool build_list();
	bool add_directory(std::string dirname);
	bool copy_object(const std::string &name);
	bool copy_object_back(const std::string &name);
	bool delete_target_object(const std::string &name);
	bool delete_source_object(const std::string &name);
};

#endif
