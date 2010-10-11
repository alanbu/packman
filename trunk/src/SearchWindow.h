/*********************************************************************
* Copyright 2009 Alan Buckley
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
	tbx::OptionButton _current;
	MainWindow *_main;

public:
	SearchWindow(tbx::Object obj);
	virtual ~SearchWindow();

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void button_selected(tbx::ButtonSelectedEvent &event);

};

#endif /* SEARCHWINDOW_H_ */
