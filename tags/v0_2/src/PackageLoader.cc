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
 * PackageLoader.cc
 *
 *  Created on: 21-Jul-2009
 *      Author: alanb
 */

#include "PackageLoader.h"
#include "Packages.h"
#include "tbx/reporterror.h"
#include "libpkg/filesystem.h"
#include "libpkg/zipfile.h"
#include "libpkg/pkgbase.h"

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
						pkgvrsn);
				pkg::copy_object(pathname, cache_pathname);
				package_base->control().insert(ctrl);
				package_base->control().commit();
			} else
			{
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
