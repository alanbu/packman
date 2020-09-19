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
 * MoveWindow.h
 *
 *  Created on: 26 Jan 2012
 *      Author: alanb
 */

#ifndef MOVEWINDOW_H_
#define MOVEWINDOW_H_

#include "tbx/command.h"
#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/slider.h"
#include "tbx/actionbutton.h"
#include "MoveApp.h"

/**
 * Class to orchestrate moving and application
 */
class MoveWindow : public tbx::Command
{
	tbx::Window _window;
	tbx::DisplayField _status_text;
	tbx::Slider _progress;
	tbx::ActionButton _cancel;
	tbx::CommandMethod<MoveWindow> _do_cancel;
	tbx::ActionButton _faster;
	tbx::CommandMethod<MoveWindow> _do_faster;

	MoveApp _move_app;
	MoveApp::State _last_state;

	bool _can_cancel;
	bool _run_faster;

public:
	MoveWindow(const std::string &logical_path, const tbx::Path &app_path, const std::string &to_path);
	virtual void execute();

private:
	void cancel();
	void faster();
	void close();
	void show_warning();
	void show_error();

	std::string warning_text();
	std::string error_text();
};

#endif /* MOVEWINDOW_H_ */
