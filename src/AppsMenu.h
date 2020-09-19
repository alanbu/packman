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
	virtual void has_been_hidden(const tbx::EventInfo &hidden_event);

public:
	AppsMenu(tbx::Object object);
	virtual ~AppsMenu();
};

#endif /* APPSMENU_H_ */
