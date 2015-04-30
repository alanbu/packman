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
 * CopyrightWindow.cc
 *
 *  Created on: 19-Aug-2009
 *      Author: alanb
 */

#include "CopyrightWindow.h"
#include "MainWindow.h"
#include "Packages.h"

#include "tbx/window.h"
#include "tbx/deleteonhidden.h"

#include "libpkg/pkgbase.h"


CopyrightWindow::CopyrightWindow(const pkg::binary_control *ctrl) : _view(tbx::Window("Copyright"), true)
{
	tbx::Window window = _view.window();

	window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<CopyrightWindow>(this));
	if (ctrl != 0)
	{
		std::string pkgname = ctrl->pkgname();

		_view.window().title("Copyright for " + pkgname);

		// Open manifest.
		pkg::pkgbase *pb = Packages::instance()->package_base();
        pkg::status_table::const_iterator sti = pb->curstat().find(ctrl->pkgname());
	    if (sti != pb->curstat().end()
	        && (*sti).second.state() == pkg::status::state_installed
	       )
	       {
	    		std::string pathname=pb->info_pathname(pkgname)+std::string(".Copyright");
	    		_view.load_file(pathname);
	       } else
	       {
	    	   std::string licence;
	           pkg::control::const_iterator found = ctrl->find("Licence");
	           if (found != ctrl->end()) licence = found->second;
	    	   std::string msg("The full copyright is downloaded when the");
	    	   msg+= " package is installed.\n\n";
	    	   msg+= "The licence is " + licence;
	    	   if (licence == "Free")
	    	   {
	    		   msg+= "\n\nFree packages ";
	               msg+= "meet the Open Source Definition, as defined by the Open Source Institute.";
	               msg+= "\n\n(See 'http://www.opensource.org/docs/definition.php').";
  	    	   } else if (licence == "Non-free")
  	    	   {
	    		   msg+= "\n\nNon-free packages ";
	    		   msg += " do not meet the Open Source Definition, as defined by the Open Source Institute.";
	    		   msg+= "\n\n(See 'http://www.opensource.org/docs/definition.php' for the Open Source Definition).";
  	    	   } else if (licence == "Unknown")
  	    	   {
  	    		   msg += "\n\nUnknown packages are usually packages created by the";
  	    		   msg += " GCCSDK Autobuilder that have not had the licence set";
  	    		   msg += " in the autobuilder build scripts.";
  	    	   }
	    	   _view.text(msg);
	       }
	}
}

CopyrightWindow::~CopyrightWindow()
{
	_view.window().delete_object();
}
