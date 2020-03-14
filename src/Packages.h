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
 * Packages.h
 *
 *  Created on: 23-Mar-2009
 *      Author: alanb
 */

#ifndef PACKAGES_H_
#define PACKAGES_H_

#include <libpkg/binary_control_table.h>

namespace pkg {

class pkgbase;
class log;
class binary_control;
class control;

}; /* namespace pkg; */

#include <vector>
#include <string>
#include <set>
#include <tr1/memory>

/**
 * Structure to identify a specific version of a package
 */
typedef pkg::binary_control_table::key_type PackageKey;

/**
 * Class to handle packages
 */
class Packages
{
	pkg::pkgbase *_package_base;
	static Packages *_instance;
	std::string _sections;
	enum {DONT_KNOW, NO, YES} _upgrades_available;
	std::vector<PackageKey> _package_list;
	bool _logging;
	std::tr1::shared_ptr<pkg::log> _log;

public:
	Packages();
	virtual ~Packages();

	static Packages *instance() {return _instance;}

	pkg::pkgbase *package_base() const {return _package_base;}

	bool ensure_package_base();
	bool reset_package_base();

	const std::vector<PackageKey> &package_list();
	void reset_package_list();
	bool status_changed();
	void environment_changed();

	std::string sections();

	const pkg::binary_control *find_control(const std::string &pkgname);

	void clear_selection();
	void select_install(const pkg::binary_control *bctrl);
	void select_install(const std::vector<std::string> &add_packages);
	void select_remove(const pkg::binary_control *bctrl);
	bool select_upgrades();
	void deselect(const std::string &pkgname);

	void unset_upgrades_available();
	bool upgrades_available();

	static std::string make_path_definition(const std::string &full_path);

	bool logging() const {return _logging;}
	void logging(bool on) {_logging = on;}

	std::tr1::shared_ptr<pkg::log> current_log() const {return _log;}
	std::tr1::shared_ptr<pkg::log> new_log();

	void get_recommendations(const std::vector< pkg::binary_control_table::key_type> &packages, std::vector<std::string> &recommends,  std::vector<std::string> &suggests);

	void check_dependencies(const pkg::control& ctrl);
private:
	void check_dependencies(const std::string &pkgname, const std::string &depends, std::string &missing, std::set<std::string> &done);
};

std::string format_description(const pkg::binary_control *bctrl);

#endif /* PACKAGES_H_ */
