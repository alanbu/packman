/*********************************************************************
* This file is part of PackMan
*
* Copyright 2015-2020 AlanBuckley
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
