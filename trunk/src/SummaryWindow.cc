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

const int SMALL_SIZE = 64;
const int LARGE_SIZE = 252;

SummaryWindow::SummaryWindow(MainWindow *main, tbx::Window main_wnd, tbx::view::Selection *selection) :
	_main(main),
	_selection(selection),
	_toggle_command(this, &SummaryWindow::on_toggle_size)
{
	tbx::Window tbwin = main_wnd.ibl_toolbar();

	_name = tbwin.gadget(1);
	_installed =tbwin.gadget(3);
	_available = tbwin.gadget(5);
	_description = tbwin.gadget(6);
	_toggle_size = tbwin.gadget(7);
	_toggle_size.add_select_command(&_toggle_command);
	_height = LARGE_SIZE;

	_selection->add_listener(this);

	// Force initial text for no selection
	set_noselection_text();
}

SummaryWindow::~SummaryWindow()
{
	_selection->remove_listener(this);
}

/**
 * Selection on main window has changed so update our details
 */
void SummaryWindow::selection_changed(const tbx::view::SelectionChangedEvent &event)
{
	if (!event.final()) return; // Only interested in last selection event

	if (!event.selected())
	{
		set_noselection_text();
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

/**
 * Set text for case where there is no selection
 */
void SummaryWindow::set_noselection_text()
{
	_name.text("(no selection)");
	_installed.text("n/a");
	_available.text("n/a");
	_description.text("");
}

/**
 * Toggle size button has been hit
 */
void SummaryWindow::on_toggle_size()
{
	tbx::Window status_window = _toggle_size.object();
	tbx::BBox visible_bounds = status_window.bounds();
	int change;

	if (_height == LARGE_SIZE)
	{
		_height = SMALL_SIZE;
		change = SMALL_SIZE - LARGE_SIZE;
		_toggle_size.on(true);
	} else
	{
		_height = LARGE_SIZE;
		_toggle_size.on(false);
		change = LARGE_SIZE - SMALL_SIZE;
	}
	visible_bounds.max.y = visible_bounds.min.y + _height;
	status_window.bounds(visible_bounds);
	tbx::Window main_window = status_window.parent_object();
	main_window.ibl_toolbar(status_window);
	_main->summary_size_changed(change);
}
