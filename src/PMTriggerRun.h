/*********************************************************************
* Copyright 2016 Alan Buckley
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
 * PMTriggerRun.h
 *
 *  Created on: 3 Dec 2015
 *      Author: alanb
 */

#ifndef PMTRIGGERRUN_H_
#define PMTRIGGERRUN_H_

#include "tbx/taskwindow.h"
#include "libpkg/trigger.h"
#include <string>

/**
 * Run trigger helper for libpkg
 */
class PMTriggerRun :
		public pkg::trigger_run,
		tbx::TaskWindowFinishedListener,
		tbx::TaskWindowOutputListener
{
	tbx::TaskWindow _task_window;
	pkg::trigger *_trigger;
public:
	PMTriggerRun();
	virtual ~PMTriggerRun();

	virtual void run(const std::string &file_name, pkg::trigger *trigger);

private:
	virtual void taskwindow_finished(tbx::TaskWindow &task_window);
	virtual void taskwindow_output(tbx::TaskWindow &task_window, int size, const char *text);
};

#endif /* PMTRIGGERRUN_H_ */
