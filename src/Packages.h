/*********************************************************************
* This file is part of PackMan
*
* Copyright 2009-2020 AlanBuckley
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
