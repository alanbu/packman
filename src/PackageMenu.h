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
