/*********************************************************************
* This file is part of PackMan
*
* Copyright 2015-2020 AlanBuckley
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
