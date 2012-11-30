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
 * Packages.h
 *
 *  Created on: 23-Mar-2009
 *      Author: alanb
 */

#ifndef PACKAGES_H_
#define PACKAGES_H_

namespace pkg {

class pkgbase;

}; /* namespace pkg; */

#include <string>
/**
 * Class to handle packages
 */
class Packages
{
	pkg::pkgbase *_package_base;
	static Packages *_instance;
	std::string _sections;
	enum {DONT_KNOW, NO, YES} _upgrades_available;

public:
	Packages();
	virtual ~Packages();

	static Packages *instance() {return _instance;}

	pkg::pkgbase *package_base() const {return _package_base;}

	bool ensure_package_base();

	std::string sections();

	void clear_selection();
	void unset_upgrades_available();
	bool upgrades_available();

	static std::string make_path_definition(const std::string &full_path);
};

#endif /* PACKAGES_H_ */
