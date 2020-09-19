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
