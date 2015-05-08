/*********************************************************************
* Copyright 2009-2015 Alan Buckley
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
			std::string depends(ctrl.depends());
			if (!depends.empty()) check_dependencies(pkgname, depends);
			if (pkgname.length() && pkgvrsn.length())
			{
				std::ostringstream out;
				out << pkg::object_length(pathname);
				ctrl["Size"] = out.str();

				std::string cache_pathname = package_base->cache_pathname(pkgname,
						pkgvrsn);
				pkg::copy_object(pathname, cache_pathname);
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

/**
 * Check that the dependencies are available and show a warning
 * if they are not.
 *
 * @param depends list of dependencies string
 */
void PackageLoader::check_dependencies(const std::string &pkgname, const std::string &depends)
{
	pkg::pkgbase *package_base = Packages::instance()->package_base();
	pkg::binary_control_table control_table = package_base->control();
	std::vector<std::vector<pkg::dependency> > depends_list;
	pkg::parse_dependency_list(depends.begin(), depends.end(), &depends_list);
	std::string missing;
	std::vector<std::vector<pkg::dependency> >::const_iterator dli;
	std::vector<pkg::dependency>::const_iterator di;

	for(dli = depends_list.begin(); dli != depends_list.end(); ++dli)
	{
		for(di = dli->begin(); di != dli->end(); ++di)
		{
			const pkg::binary_control &dctrl = control_table[di->pkgname()];
			if (dctrl.pkgname() != di->pkgname())
			{
				if (!missing.empty()) missing += ", ";
				missing += di->pkgname();
			} else if (!di->matches(dctrl.pkgname(),pkg::version(dctrl.version())))
			{
				if (!missing.empty()) missing += ", ";
				missing += di->pkgname() + "version " + di->version().package_version();
			}
		}
	}
	if (!missing.empty())
	{
		missing = "Warning: The following dependencies are not available\n" + missing;
		missing += "\n\nPlease add them before installing this package.";
		std::string title("Missing dependencies for ");
		title += pkgname;
		tbx::show_message(missing, title, "warning");
	}
}
