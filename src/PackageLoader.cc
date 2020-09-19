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
 * PackageLoader.cc
 *
 *  Created on: 21-Jul-2009
 *      Author: alanb
 */

#include "PackageLoader.h"
#include "Packages.h"
#include "PackageConfigWindow.h"
#include "PackManState.h"
#include "tbx/reporterror.h"
#include "tbx/path.h"
#include "libpkg/filesystem.h"
#include "libpkg/zipfile.h"
#include "libpkg/pkgbase.h"
#include "tbx/hourglass.h"
#include "tbx/messagewindow.h"

#include <sstream>

PackageLoader::PackageLoader()
{
}

PackageLoader::~PackageLoader()
{
}

/**
 * Add package to the list of packages
 */
bool PackageLoader::load_file(tbx::LoadEvent &event)
{
   if (event.from_filer() && Packages::instance()->ensure_package_base())
   {
		pkg::pkgbase *package_base = Packages::instance()->package_base();

		try
		{
			tbx::Hourglass hg;
			std::string pathname(event.file_name());
			pkg::binary_control ctrl = extract_control(pathname);
			std::string pkgname(ctrl.pkgname());
			std::string pkgvrsn(ctrl.version());
			if (pkgname.length() && pkgvrsn.length())
			{
				std::ostringstream out;
				out << pkg::object_length(pathname);
				ctrl["Size"] = out.str();

				std::string cache_pathname = package_base->cache_pathname(pkgname,
						pkgvrsn, ctrl.environment_id());
				tbx::Path(pathname).copy(cache_pathname,tbx::Path::COPY_FORCE);
				package_base->control().insert(ctrl);
				package_base->control().commit();

				// Attempt to show dragged item in the configuration window
				// immediately
				try
				{
					pkg::status_table::const_iterator sti = package_base->curstat().find(pkgname);
					bool can_show = false;
					if (sti == package_base->curstat().end()
						  || (*sti).second.state() != pkg::status::state_installed
						  )
					{
						can_show = true; // Not installed so we can install it now
					} else
					{
						  pkg::version inst_version((*sti).second.version());
						  pkg::version cur_version(pkgvrsn);
						  can_show = (inst_version < cur_version);
					}
					if (can_show)
					{
						if (pmstate()->ok_to_commit())
						{
							Packages::instance()->select_install(&ctrl);
							PackageConfigWindow::update();
						}
					} else
					{
						hg.off();
						std::string msg;
						msg = "Version " + pkgvrsn + " or later of package " + pkgname + " is already installed";
					   tbx::report_error(msg);
					}
				} catch (...)
				{
					hg.off();
					tbx::report_error("Unable to select Package for configuration window");
				}
			} else
			{
				hg.off();
				tbx::report_error("Not a package (Package name or version not found)", 0);
				return false;
			}

			return true;
		} catch (pkg::zipfile::not_found& ex)
		{
			tbx::report_error("Not a package (control file not found)", 0);
		} catch (std::exception& ex)
		{
			tbx::report_error("Failed to load package", 0);
		}
   }

   return false;
}

/**
 * Extract the control record from the given file
 */
pkg::binary_control PackageLoader::extract_control(const std::string& pathname)
{
	pkg::create_directory("<Wimp$ScrapDir>.PackMan");
	pkg::zipfile zf(pathname);
	zf.extract("RiscPkg/Control","<Wimp$ScrapDir>.PackMan.Control");
	std::ifstream ctrlfile("<Wimp$ScrapDir>.PackMan.Control");
	pkg::binary_control ctrl;
	ctrlfile >> ctrl;
	return ctrl;
}

