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
 * SummaryWindow.cc
 *
 *  Created on: 30-Mar-2009
 *      Author: alanb
 */

#include "SummaryWindow.h"
#include "MainWindow.h"
#include "Packages.h"
#include "libpkg/pkgbase.h"
#include "tbx/font.h"

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

   // RISC OS 5 was failing to format correctly unless the font was set
   tbx::Font dtf;
   if (dtf.desktop_font())
   {
      tbx::FullFontDetails details;
      dtf.read_details(details);
      _description.font(details.identifier, details.x_point_size, details.y_point_size);
   } else
   {
      _description.system_font(208,208);
   }

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
	    set_selection_text(true);
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
 * Set/update the text for the selected item
 *
 * @param description true if description needs to be updated also
 */
void SummaryWindow::set_selection_text(bool description)
{
    const pkg::binary_control *bctrl = _main->selected_package();
    _name.text(bctrl->pkgname());
    pkg::pkgbase *package_base = Packages::instance()->package_base();
    pkg::status_table::const_iterator sti = package_base->curstat().find(bctrl->pkgname());
    if(sti == package_base->curstat().end() || (*sti).second.state() != pkg::status::state_installed)
    {
        _installed.text("No");
        _available.text(bctrl->version());
    }else
    {
        _installed.text((*sti).second.version());
		  pkg::env_packages_table::const_iterator epi = package_base->env_packages().find(bctrl->pkgname());
		  if (epi == package_base->env_packages().end())
		  {
		        _available.text(bctrl->version());
		  } else
		  {
		        _available.text(epi->second.pkgvrsn);
		  }
    }

    if (description)
    {
    	// Updating selection can set focus to window, so it is
    	// sometimes omitted if when the selection hasn't changed
    	_description.text(format_description(bctrl));
    	_description.set_selection(0, 0);
    }
}

/**
 * Check if summary bar is at it's small size
 */
bool SummaryWindow::is_small_size() const
{
	return (_height == SMALL_SIZE);
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
