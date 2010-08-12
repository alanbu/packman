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
 * InfoWindow.cc
 *
 *  Created on: 24-Jul-2009
 *      Author: alanb
 */

#include "InfoWindow.h"
#include "MainWindow.h"
#include "Packages.h"
#include "libpkg/pkgbase.h"
#include "tbx/objectdelete.h"
#include "tbx/scrolllist.h"

const int NumDisplayFields = 9;

static char*DisplayFields[] =
{
		"Package",
		"Version",
		"Section",
		"Priority",
		"Size",
		"URL",
		"Source",
		"Standards-Version",
		"Maintainer"
};

const int NumDepFields = 4;
static char *DepFields[] =
{
		"Depends",
		"Conflicts",
		"Recommends",
		"Suggests"
};

InfoWindow::InfoWindow(tbx::Object object) :
	_window(object),
	_installed(_window.gadget(0x22)),
	_description(_window.gadget(0x21))
{
	_window.add_about_to_be_shown_listener(this);

	// Ensure class is deleted if object is
	_window.add_object_deleted_listener(new tbx::ObjectDeleteClass<InfoWindow>(this));
}

InfoWindow::~InfoWindow()
{
}

/**
 * Find currently selected package and update menu
 */
void InfoWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	MainWindow *main = MainWindow::from_window(event.id_block().ancestor_object());
	const pkg::binary_control *ctrl = main->selected_package();

	if (ctrl == 0) return; // This shouldn't happen

    int j;

    for (j = 0; j < NumDisplayFields; j++)
    {
    	tbx::DisplayField fld = _window.gadget(1+j*2);
    	pkg::control::const_iterator found = ctrl->find(DisplayFields[j]);
    	if (found == ctrl->end())
    	{
    		fld.text("");
    	} else
    	{
    		fld.text(found->second);
    	}
    }

    for (j = 0; j < NumDepFields; j++)
    {
    	tbx::ScrollList fld = _window.gadget(0x19+j*2);
    	pkg::control::const_iterator found = ctrl->find(DepFields[j]);
    	if (found == ctrl->end())
    	{
    		fld.clear();
    	} else
    	{
    		std::string deps(found->second);
    		std::string::size_type start = 0, pos;
    		while ((pos = deps.find(',', start)) != std::string::npos)
    		{
    			fld.add_item(deps.substr(start, pos-start));
    			start = pos + 1;
    			while (start < deps.size() && deps[start] == ' ') start++;
    		}
    		fld.add_item(deps.substr(start));
    	}
    }

    // Installed information
	pkg::pkgbase *package_base = Packages::instance()->package_base();
	pkg::status_table::const_iterator sti = package_base->curstat().find(ctrl->pkgname());
	if (sti == package_base->curstat().end()
		  || (*sti).second.state() != pkg::status::state_installed
		 )
	{
		_installed.text("No");
	} else
	{
		_installed.text((*sti).second.version());
	}

	_description.text(ctrl->description());
	_description.set_selection(0,0);
}
