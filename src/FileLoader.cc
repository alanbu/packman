/*********************************************************************
* Copyright 2015 Alan Buckley
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
 * FileLoader.c
 *
 *  Created on: 29 Jun 2015
 *      Author: alanb
 */

#include "FileLoader.h"
#include "Packages.h"
#include "PackageLoader.h"
#include "FindWindow.h"
#include "InstallListLoadWindow.h"

#include <fstream>
#include <cstring>

FileLoader::FileLoader()
{
}

FileLoader::~FileLoader()
{
}

/**
 * Check file contents and route it to the correct
 * functionality in PackMan
 */
bool FileLoader::load_file(tbx::LoadEvent &event)
{
	if (event.from_filer() && Packages::instance()->ensure_package_base())
	{
		std::ifstream fil(event.file_name(), std::ifstream::binary);
		bool package = false;
		bool package_list = false;
		if (fil)
		{
			char buffer[13];
			fil.read(buffer, 13);
			if (fil && !fil.eof())
			{
				package = ( buffer[0] == 'P' && buffer[1] == 'K' );
				if (!package && std::strncmp(buffer, "<packagelist ",13) == 0) package_list = true;
			}
			fil.close();
		}
		if (package)
		{
			PackageLoader pl;
			return pl.load_file(event);
		} else if (package_list)
		{
			new InstallListLoadWindow(event.file_name());
		} else
		{
			new FindWindow(event.file_name());
		}
	}

	return false;
}
