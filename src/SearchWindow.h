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
 * SearchWindow.h
 *
 *  Created on: 20-Aug-2009
 *      Author: alanb
 */

#ifndef SEARCHWINDOW_H_
#define SEARCHWINDOW_H_

#include "tbx/abouttobeshownlistener.h"
#include "tbx/window.h"
#include "tbx/writablefield.h"
#include "tbx/optionbutton.h"
#include "tbx/buttonselectedlistener.h"

class MainWindow;

/**
 * Window to get parameters for search
 */
class SearchWindow :
	tbx::AboutToBeShownListener,
	tbx::ButtonSelectedListener
{
	tbx::Window _window;
	tbx::WritableField _text;
	tbx::OptionButton _current_status;
	tbx::OptionButton _current_section;
	MainWindow *_main;

public:
	SearchWindow(tbx::Object obj);
	virtual ~SearchWindow();

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void button_selected(tbx::ButtonSelectedEvent &event);

};

#endif /* SEARCHWINDOW_H_ */
