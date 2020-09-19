/*********************************************************************
* This file is part of PackMan
*
* Copyright 2009-2020 AlanBuckley
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
 * BackupAndRun.h
 *
 *  Created on: 18 May 2012
 *      Author: alanb
 */

#ifndef BACKUPANDRUN_H_
#define BACKUPANDRUN_H_

#include "tbx/command.h"
#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/slider.h"
#include "tbx/actionbutton.h"

#include "FSObjectCopy.h"

#include <deque>
#include <stack>
#include <vector>

/**
 * Class to run a backup and if it's successful follow it
 * by running a command.
 */
class BackupAndRun : public tbx::Command
{
	std::string _title;
	tbx::Command *_run_command;
	std::deque<FSObjectCopy *> _backups;
	std::stack<FSObjectCopy *> _backups_done;

	tbx::Window _window;
	tbx::DisplayField _status_text;
	tbx::Slider _progress;
	tbx::ActionButton _cancel;
	tbx::CommandMethod<BackupAndRun> _do_cancel;
	tbx::ActionButton _faster;
	tbx::CommandMethod<BackupAndRun> _do_faster;

	bool _can_cancel;
	bool _run_faster;

	enum State {START_BACKUP, BACKUP_NEXT, BACKUP_FILES, UNWIND_NEXT, UNWIND_BACKUP, FAILED, DONE};
	State _state, _last_state;

	enum Error {NO_ERROR, BACKUP_FAILED, BACKUP_AND_UNWIND_FAILED};
	Error _error;
	std::string _errmsg;

	long long _cost_to_backup;
	long long _cost_done;
	long long _cost_total;
	bool _cancelled;

public:
	BackupAndRun(const std::string &title, tbx::Command *command);
	virtual ~BackupAndRun();

	void add(const tbx::Path &backup_object);
	void add_children(const tbx::Path &backup_root, const std::vector<std::string> &children);
	void start();

	virtual void execute();

private:
	bool process();
	void cancel();
	void faster();
	void close();
};

#endif /* BACKUPANDRUN_H_ */
