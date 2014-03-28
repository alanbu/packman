/*********************************************************************
* Copyright 2009-2014 Alan Buckley
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
#include "libpkg/component.h"
#include "tbx/objectdelete.h"
#include "tbx/scrolllist.h"
#include "tbx/font.h"

#include <sstream>

InfoWindow *InfoWindow::_instance = 0;

// Extra OS units required on top of text length for a scroll list (approx)
const int ScrollListExtra = 80;
// OS units margin on right of window (use when it's expanded)
const int RightMargin = 16;

const int NumDisplayFields = 10;

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
               "Maintainer",
               "Licence"
};

const int NumDepFields = 4;
static char *DepFields[] =
{
               "Depends",
               "Conflicts",
               "Recommends",
               "Suggests"
};

InfoWindow::InfoWindow() :
       _window("Info"),
       _installed(_window.gadget(0x22)),
       _description(_window.gadget(0x21)),
       _components(_window.gadget(0x2e))
{
  _instance = this;
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
}

InfoWindow::~InfoWindow()
{
	_instance = 0;
}

/**
 * Show information for the binary control record
 *
 * @param ctrl binary control record to show
 */
void InfoWindow::show(const pkg::binary_control *ctrl)
{
	if (_instance == 0) new InfoWindow();
	_instance->_window.show();
	_instance->update_details(ctrl);
}

/**
 * Update the information window
 */
void InfoWindow::update_details(const pkg::binary_control *ctrl)
{
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
               fld.clear();
       if (found != ctrl->end())
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

       _description.text(format_description(ctrl));
       _description.set_selection(0,0);

       std::string comps = ctrl->components();
       _components.clear();
       if (!comps.empty())
       {
    	   try
    	   {
			   std::vector<pkg::component> comp_list;
			   pkg::path_table &paths = package_base->paths();
			   tbx::WimpFont wf;
			   int max_width = 0, width;
			   std::string comp_path;
			   pkg::parse_component_list(comps.begin(), comps.end(), &comp_list);
			   for(std::vector<pkg::component>::iterator c = comp_list.begin(); c != comp_list.end();++c)
			   {
				   comp_path = paths(c->name(),"");
				   _components.add_item(comp_path);
				   width = wf.string_width_os(comp_path);
				   if (width > max_width) max_width = width;
			   }

			   // Check if we need to resize to fit everything
			   tbx::BBox comp_bounds = _components.bounds();
			   if (max_width + ScrollListExtra > comp_bounds.width())
			   {
				   comp_bounds.max.x = comp_bounds.min.x + max_width + ScrollListExtra;
				   _components.bounds(comp_bounds);
				   tbx::BBox extent = _window.extent();
				   if (comp_bounds.max.x + RightMargin > extent.max.x)
				   {
					   extent.max.x = comp_bounds.max.x + RightMargin;
					   _window.extent(extent);
				   }
			   }
    	   } catch(...)
    	   {
    		   // Ignore errors
    	   }
       }
}