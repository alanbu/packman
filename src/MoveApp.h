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

#ifndef MOVEAPP_H
#define MOVEAPP_H

#include "tbx/path.h"
#include "FSObjectCopy.h"
#include "ComponentsMoved.h"

/**
 * Class to move an application on the disc and
 * update the package manager to the new location
 *
 * To use:
 *  Construct with from and to paths
 *  Repeatedly call poll() until status() is DONE or FAILED;
 *  If an error occurs FAILED is returned at the end and error()
 *  is set with the reason
 *  If it failed and couldn't restore things to the original state or
 *  could not delete the original app after the move a warning is
 *  return in warning()
 */
class MoveApp
{
public:
	enum State {START_BACKUP, BACKUP_FILES, START, COPYING_FILES, UPDATE_PATHS, UPDATE_VARS, UPDATE_BOOT_OPTIONS, DELETE_OLD_FILES, DONE, UNWIND_COPY, UNWIND_BACKUP, FAILED};
	enum Error {NO_ERROR, COPY_FAILED, PATH_UPDATE_FAILED, BACKUP_FAILED, CANCELLED};
	enum Warning {NO_WARNING, DELETE_FAILED, UNWIND_PATHS_FAILED, UNWIND_COPY_FAILED, UNWIND_BACKUP_FAILED};

private:
	std::string _logical_path;
	FSObjectCopy _copy_handler;
	FSObjectCopy *_backup_handler;
	ComponentsMoved *_comps_moved;
	State _state;
	Error _error;
	Warning _warning;
	long long _cost_done;
	long long _cost_total;
	long long _cost_stage_start;
	bool _can_cancel;
	bool _cancelled;
	
public:
	MoveApp(const std::string &logical_path, const std::string &app_path, const std::string &to_path);
	~MoveApp();

	void poll();

	State state() const {return _state;}
	Error error() const {return _error;}
	Warning warning() const {return _warning;}
	int decipercent_done() const;

	bool can_cancel() const {return _can_cancel;}
	bool cancelled() const {return _cancelled;}
	void cancel();

private:
	void setup_backup(const tbx::Path &target);
	void calc_cost_total(bool backup_only);
};

#endif
