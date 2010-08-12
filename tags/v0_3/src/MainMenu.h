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
 * MainMenu.h
 *
 *  Created on: 23-Jul-2009
 *      Author: alanb
 */

#ifndef MAINMENU_H_
#define MAINMENU_H_

#include "tbx/menu.h"
#include "tbx/abouttobeshownlistener.h"

/**
 * Class to handle the main menu
 */
class MainMenu : tbx::AboutToBeShownListener
{
	tbx::Menu _menu;
	tbx::MenuItem _package_item;

public:
	MainMenu(tbx::Object object);
	virtual ~MainMenu();

	void about_to_be_shown(tbx::AboutToBeShownEvent &event);
};

#endif /* MAINMENU_H_ */
