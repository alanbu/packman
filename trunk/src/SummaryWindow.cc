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
 * SummaryWindow.cc
 *
 *  Created on: 30-Mar-2009
 *      Author: alanb
 */

#include "SummaryWindow.h"
#include "MainWindow.h"
#include "Packages.h"
#include "libpkg/pkgbase.h"


SummaryWindow::SummaryWindow(MainWindow *main, tbx::Window main_wnd, SingleSelection *selection) :
	_main(main),
	_selection(selection)
{
	tbx::Window tbwin = main_wnd.ibl_toolbar();

	_name = tbwin.gadget(1);
	_installed =tbwin.gadget(3);
	_available = tbwin.gadget(5);
	_description = tbwin.gadget(6);

	_selection->add_listener(this);

	// Force initial text for no selection
	selection_changed(SingleSelection::none, SingleSelection::none);
}

SummaryWindow::~SummaryWindow()
{
	_selection->remove_listener(this);
}

/**
 * Selection on main window has changed so update our details
 */
void SummaryWindow::selection_changed(unsigned int old_index, unsigned int new_index)
{
	if (new_index == SingleSelection::none)
	{
		_name.text("(no selection)");
		_installed.text("n/a");
		_available.text("n/a");
		_description.text("");
	} else
	{
		const pkg::binary_control *bctrl = _main->selected_package();

		_name.text(bctrl->pkgname());

		pkg::pkgbase *package_base = Packages::instance()->package_base();
		pkg::status_table::const_iterator sti = package_base->curstat().find(bctrl->pkgname());
		if (sti == package_base->curstat().end()
			  || (*sti).second.state() != pkg::status::state_installed
			 )
		  {
			  _installed.text("No");
		  } else
		  {
			 _installed.text((*sti).second.version());
		  }

		  _available.text(bctrl->version());

		_description.text(bctrl->description());
		_description.set_selection(0,0);
	}
}
