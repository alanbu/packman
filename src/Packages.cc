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
 * Packages.cc
 *
 *  Created on: 23-Mar-2009
 *      Author: alanb
 */

#include "Packages.h"

#include "libpkg/pkgbase.h"
#include "libpkg/filesystem.h"
#include <string>
#include <set>

#include "swis.h"
#include "tbx/swixcheck.h"

using namespace std;
string canonicalise(string pathname)
{
	printf("in canon\n");
	// Calculate required buffer size.
	unsigned int size=0;
	_kernel_swi_regs regs;
	_kernel_oserror *err;
	regs.r[0] = 37;
	regs.r[1]= (int)pathname.c_str();
	regs.r[2] =0;
	regs.r[3] =0;
	regs.r[4] =0;
	regs.r[5] = size;

	printf("pre call\n");
	tbx::swix_check( _kernel_swi(OS_FSControl, &regs, &regs));
	printf("post call err = %p\n", err);
	size = regs.r[5];
	size=1-size;
printf("size is %d\n", size);
	// Canonicalise pathname.
	char buffer[size];
	regs.r[0] = 37;
	regs.r[1]= (int)pathname.c_str();
	regs.r[2] =(int)buffer;
	regs.r[3] =0;
	regs.r[4] =0;
	regs.r[5] = size;

	printf("pre call\n");
	err = _kernel_swi(OS_FSControl, &regs, &regs);
	printf("post call err = %p\n", err);
printf("pre return\n");
	return string(buffer);
}


Packages *Packages::_instance = 0;

Packages g_packages; // Single instance of the packages

Packages::Packages() : _package_base(0)
{
	_instance = this;
}

Packages::~Packages()
{
	_instance = 0;
}

/**
 * Ensure packages have been loaded
 */
bool Packages::ensure_package_base()
{
   	if (!_package_base)
	{
   		printf("!pb\n");
		try
		{
			// Do not use distribution master of package database.
			printf("pre apath\n");
			std::string apath=/*pkg::*/canonicalise("<Packages$Dir>");
			printf("now here\n");
			std::string dpath=
				/*pkg::*/canonicalise("<PackMan$Dir>.Resources.!Packages");
			printf("here\n");
			if ((apath!=dpath)&&(pkg::object_type(apath)!=0))
			{
				// Attempt to access package database.
				_package_base=new pkg::pkgbase("<Packages$Dir>","<RiscPkg$Dir>.Resources",
					"Choices:RiscPkg");
			}
		} catch(tbx::OsError &err)
		{
			printf("Got os error\n");
		} catch (...)
		{
			printf("package db failed\n");
			delete _package_base;
			_package_base=0;
		}
	}
	return (_package_base != 0);
}

/**
 * Return a sorted, comma separated list of all the sections
 * in the given packages
 */
std::string Packages::sections()
{
	if (_sections.empty())
	{
		const pkg::binary_control_table& ctrltab = _package_base->control();
		std::set<std::string> section_set;

		pkg::control::key_type section_key("Section");

		for (pkg::binary_control_table::const_iterator i=ctrltab.begin();
			 i !=ctrltab.end(); ++i)
		{
		    const pkg::binary_control &ctrl = i->second;

			pkg::control::const_iterator s = ctrl.find(section_key);
			if (s != ctrl.end()) section_set.insert((*s).second);
		}

		for (std::set<std::string>::iterator entry = section_set.begin();
		     entry != section_set.end(); ++entry)
		{
			_sections += (*entry);
			_sections += ",";
		}
		// Remove extra comma
		if (!_sections.empty()) _sections.erase(_sections.length()-1);
	}

	return _sections;
}
