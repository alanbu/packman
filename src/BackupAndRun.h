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
