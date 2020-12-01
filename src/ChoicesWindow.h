/*********************************************************************
* This file is part of PackMan
*
* Copyright 2014-2020 AlanBuckley
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
 * ChoicesWindow.h
 *
 *  Created on: 6 Aug 2014
 *      Author: alanb
 */

#ifndef CHOICESWINDOW_H_
#define CHOICESWINDOW_H_

#include "tbx/abouttobeshownlistener.h"
#include "tbx/buttonselectedlistener.h"
#include "tbx/optionbuttonstatelistener.h"
#include "tbx/window.h"
#include "tbx/stringset.h"
#include "tbx/optionbutton.h"
#include "tbx/writablefield.h"

/**
 * Dialog to allow
 */
class ChoicesWindow :
		tbx::AboutToBeShownListener,
		tbx::ButtonSelectedListener,
		tbx::OptionButtonStateListener
{
	tbx::Window _window;
	tbx::StringSet _update_list_prompt;
	tbx::OptionButton _enable_logging;
    tbx::OptionButton _use_proxy;
	tbx::WritableField _proxy_server;
	tbx::WritableField _do_not_proxy;
	
	static ChoicesWindow* _instance;

public:
	ChoicesWindow();
	virtual ~ChoicesWindow();

	static void show();

private:
	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void button_selected(tbx::ButtonSelectedEvent &event);
	virtual void option_button_state_changed(tbx::OptionButtonStateEvent &event);

};

#endif /* CHOICESWINDOW_H_ */
