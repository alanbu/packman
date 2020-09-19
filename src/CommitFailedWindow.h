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
 * CommitFailedWindow.h
 *
 *  Created on: 19-Aug-2009
 *      Author: alanb
 */

#ifndef COMMITFAILEDWINDOW_H_
#define COMMITFAILEDWINDOW_H_

#include <string>
#include "tbx/actionbutton.h"
#include "tbx/command.h"
#include <tr1/memory>

namespace pkg
{
   class commit;
   class log;
}

class LogViewer;

/**
 * Class to show failure message from install/remove
 */
class CommitFailedWindow
{
	tbx::ActionButton _show_log;
	/** The log file for this operation */
	std::tr1::shared_ptr<pkg::log> _log;
	/** The log viewer shown from this window */
	LogViewer *_log_viewer;

	/** Binding from log button to show log command */
	tbx::CommandMethod<CommitFailedWindow> _show_log_command;

public:
	CommitFailedWindow(pkg::commit *commit, std::string last_action, std::tr1::shared_ptr<pkg::log> commit_log);
	virtual ~CommitFailedWindow();

private:
	void show_log();
};

#endif /* COMMITFAILEDWINDOW_H_ */
