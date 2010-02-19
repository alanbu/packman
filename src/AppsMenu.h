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
 * AppsMenu.h
 *
 *  Created on: 30-Jul-2009
 *      Author: alanb
 */

#ifndef APPSMENU_H_
#define APPSMENU_H_

#include "tbx/menu.h"
#include "tbx/abouttobeshownlistener.h"
#include "tbx/hasbeenhiddenlistener.h"


/**
 * Class to handle AppsMenu shown from AppsWindow.
 * Also enables/disables items on AppMenu submenu
 */
class AppsMenu :
	tbx::AboutToBeShownListener,
	tbx::HasBeenHiddenListener
{
	tbx::Menu _apps_menu;
	tbx::MenuItem _app_item;

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void has_been_hidden(tbx::Object &object);

public:
	AppsMenu(tbx::Object object);
	virtual ~AppsMenu();
};

#endif /* APPSMENU_H_ */
