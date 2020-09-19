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
	tbx::MenuItem _upgrade_all_item;

public:
	MainMenu(tbx::Object object);
	virtual ~MainMenu();

	void about_to_be_shown(tbx::AboutToBeShownEvent &event);
};

#endif /* MAINMENU_H_ */
