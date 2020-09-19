/*********************************************************************
* This file is part of PackMan
*
* Copyright 2013-2020 AlanBuckley
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
 * BootOptionsWindow.h
 *
 *  Created on: 9 Oct 2013
 *      Author: alanb
 */

#ifndef BOOTOPTIONSWINDOW_H_
#define BOOTOPTIONSWINDOW_H_

#include "tbx/abouttobeshownlistener.h"
#include "tbx/command.h"
#include "tbx/window.h"
#include "tbx/optionbutton.h"

/**
 * Class to show and allow changing of the current boot options
 */
class BootOptionsWindow : tbx::AboutToBeShownListener
{
	tbx::Window _window;
	tbx::OptionButton _look_at_option;
	tbx::OptionButton _run_option;
	tbx::OptionButton _add_to_apps_option;
	tbx::CommandMethod<BootOptionsWindow> _apply_command;

	std::string _app_path;
	bool _look_at;
	bool _run;
	bool _add_to_apps;

public:
	BootOptionsWindow(tbx::Object window);
	virtual ~BootOptionsWindow();

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);

	void apply();
};

#endif /* BOOTOPTIONSWINDOW_H_ */
