/*********************************************************************
* Copyright 2014 Alan Buckley
*
* This file is part of PkgTest (PackMan unit testing)
*
* PackMan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* PkgTest is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with PackMan. If not, see <http://www.gnu.org/licenses/>.
*
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
