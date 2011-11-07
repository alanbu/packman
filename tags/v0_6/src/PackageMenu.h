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
 * PackageMenu.h
 *
 *  Created on: 20-Aug-2009
 *      Author: alanb
 */

#ifndef PACKAGEMENU_H_
#define PACKAGEMENU_H_

#include "tbx/menu.h"
#include "tbx/abouttobeshownlistener.h"

/**
 * Class to update package menu items
 */
class PackageMenu : tbx::AboutToBeShownListener
{
private:
	tbx::Menu _menu;
	tbx::MenuItem _install;
	static const int _num_installed_items = 3;
	tbx::MenuItem _installed_items[_num_installed_items];

public:
	PackageMenu(tbx::Object object);
	virtual ~PackageMenu();

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
};

#endif /* PACKAGEMENU_H_ */
