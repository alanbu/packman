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
 * MovePathWindow.h
 *
 *  Created on: 19 Aprn 2012
 *      Author: alanb
 */

#ifndef MOVEPATHWINDOW_H_
#define MOVEPATHWINDOW_H_

#include "tbx/command.h"
#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/slider.h"
#include "tbx/actionbutton.h"
#include "MovePath.h"

class MovePathWindow : public tbx::Command
{
	tbx::Window _window;
	tbx::DisplayField _status_text;
	tbx::Slider _progress;
	tbx::ActionButton _cancel;
	tbx::CommandMethod<MovePathWindow> _do_cancel;
	tbx::ActionButton _faster;
	tbx::CommandMethod<MovePathWindow> _do_faster;

	MovePath _move_path;
	MovePath::State _last_state;

	bool _can_cancel;
	bool _run_faster;

public:
	MovePathWindow(const std::string &logical_path, const std::string &to_path);
	virtual void execute();

private:
	void cancel();
	void close();
	void show_warning();
	void show_error();
	void faster();

	std::string warning_text();
	std::string error_text();
};

#endif /* MOVEPATHWINDOW_H_ */
