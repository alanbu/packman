/*********************************************************************
* This file is part of PkgTest (PackMan unit testing)
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
 * AddTestWnd.h
 *
 *  Created on: 19 Feb 2014
 *      Author: alanb
 */

#ifndef ADDTESTWND_H_
#define ADDTESTWND_H_

#include "tbx/window.h"
#include "tbx/writablefield.h"
#include "tbx/command.h"
#include "tbx/abouttobeshownlistener.h"


/**
 * Class to handle adding a new test window
 */
class AddTestWnd : tbx::AboutToBeShownListener
{
	tbx::Window _window;
	tbx::WritableField _name_field;
	tbx::WritableField _desc_field;
	tbx::CommandMethod<AddTestWnd> _add_command;

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &e);
public:
	AddTestWnd(tbx::Object obj);
	virtual ~AddTestWnd();

	void add();
};

#endif /* ADDTESTWND_H_ */
