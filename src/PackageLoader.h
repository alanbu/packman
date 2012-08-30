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
 * PackageLoader.h
 *
 *  Created on: 21-Jul-2009
 *      Author: alanb
 */

#ifndef PACKAGELOADER_H_
#define PACKAGELOADER_H_

#include "tbx/loader.h"
#include "libpkg/binary_control.h"

/**
 * Class to add a package to the list of packages
 * from the local file system when it is dropped
 * on the iconbar
 */
class PackageLoader : public tbx::Loader
{
public:
	PackageLoader();
	virtual ~PackageLoader();

    virtual bool load_file(tbx::LoadEvent &event);

private:
	pkg::binary_control extract_control(const std::string& pathname);
};

#endif /* PACKAGELOADER_H_ */
