/*********************************************************************
* Copyright 2011 Alan Buckley
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
 * CreateStub.h
 *
 *  Created on: 11 Oct 2011
 *      Author: alanb
 */

#ifndef CREATESTUB_H_
#define CREATESTUB_H_
#include "tbx/path.h"

/** Create obey file to link from stub to target.
 * Take no action if the target file does not exist.
 *
 * @param source_path path to location of the file to link to
 * @param path the location to create the link
 * @param leaf the item to link
 */
inline void create_file_stub(const tbx::Path &source_path, const tbx::Path &path, std::string leaf)
{
	tbx::Path src(source_path, leaf);
	tbx::Path dst(path, leaf);
	if (src.exists())
	{
		std::string contents("/");
		contents += src.name();
		contents += " %*0\n";
		dst.save_file(contents.c_str(), contents.size(), 0xfeb);
	}
}

/**
 * Create an application directory stub that puts obey
 * files in it to invoke the files in the original
 * application.
 *
 * @param source_path path to the application to link to
 * @param target_path path for the application stub
 */
inline void create_application_stub(const tbx::Path &source_path, const tbx::Path &target_path)
{
	if (target_path.create_directory())
	{
		create_file_stub(source_path, target_path, "!Boot");
		create_file_stub(source_path, target_path, "!Run");
		create_file_stub(source_path, target_path, "!Help");
	}
}

#endif /* CREATESTUB_H_ */
