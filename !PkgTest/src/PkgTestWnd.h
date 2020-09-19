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

#ifndef _PkgTestWND_H_
#define _PkgTestWND_H_

#include "tbx/autocreatelistener.h"
#include "tbx/abouttobeshownlistener.h"
#include "tbx/scrolllist.h"
#include "tbx/optionbutton.h"
#include "tbx/command.h"
#include "TestRunner.h"

/**
 * Class to set up listeners an variables when the single
 * window is shown.
 */
class PkgTestWnd :
		public tbx::AutoCreateListener
{
	tbx::ScrollList _tests_list;
	tbx::OptionButton _add_pkg_log;
	tbx::OptionButton _rebuild_disc;
	tbx::CommandMethod<PkgTestWnd> _run_all_command;
	tbx::CommandMethod<PkgTestWnd> _run_selected_command;
	tbx::CommandMethod<PkgTestWnd> _edit_script_command;
	TestList _tests;

	class UpdateMenu : public tbx::AboutToBeShownListener
	{
		PkgTestWnd *_me;
	public:
		UpdateMenu(PkgTestWnd *me) : _me(me) {}
		virtual ~UpdateMenu() {}
		virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	} _update_menu;

public:
	PkgTestWnd();

	static PkgTestWnd *from_window(tbx::Window w);

	TestList &tests() {return _tests;}
	void tests_updated();

    virtual void auto_created(std::string template_name, tbx::Object object);

    void edit_script();
	void run_all();
	void run_selected();


};

#endif
