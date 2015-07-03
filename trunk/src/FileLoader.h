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
 * FileLoader.h
 *
 *  Created on: 29 Jun 2015
 *      Author: alanb
 */

#ifndef FILELOADER_H_
#define FILELOADER_H_

#include <tbx/loader.h>

/**
 * Class to look at file dropped on icon and redirect it
 * to the code to load it depending on it's contents
 */
class FileLoader: public tbx::Loader
{
public:
	FileLoader();
	virtual ~FileLoader();

	virtual bool load_file(tbx::LoadEvent &event);
};

#endif /* FILELOADER_H_ */
