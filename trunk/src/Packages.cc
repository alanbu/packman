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
 * Packages.cc
 *
 *  Created on: 23-Mar-2009
 *      Author: alanb
 */

#include "Packages.h"
#include "ErrorWindow.h"
#include "Choices.h"
#include "libpkg/pkgbase.h"
#include "libpkg/filesystem.h"
#include "libpkg/log.h"
#include <string>
#include <set>
#include <cstdlib>

#include <iostream>

#include "swis.h"
#include "tbx/application.h"
#include "tbx/swixcheck.h"
#include "tbx/stringutils.h"
#include "tbx/messagewindow.h"

using namespace std;

Packages *Packages::_instance = 0;

Packages g_packages; // Single instance of the packages

Packages::Packages() :
     _package_base(0),
     _upgrades_available(DONT_KNOW),
     _logging(choices().enable_logging())
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
		try
		{
			// Do not use distribution master of package database.
			std::string apath=pkg::canonicalise("<Packages$Dir>");
			std::string dpath=
				pkg::canonicalise("<PackMan$Dir>.Resources.!Packages");
			if ((apath!=dpath)&&(pkg::object_type(apath)!=0))
			{
				// If the package database exists, but does not contain a
				// Paths file, and a Paths file does exist in the choices
				// directory (as used by previous versions of RiscPkg)
				// then copy it across.
				string pb_ppath=apath+string(".Paths");
				string ch_ppath("Choices:RiscPkg.Paths");
				if ((pkg::object_type(pb_ppath)==0)&&
					(pkg::object_type(ch_ppath)!=0))
				{
					pkg::copy_object(ch_ppath,pb_ppath);
				}

				// Attempt to access package database.
				_package_base=new pkg::pkgbase("<Packages$Dir>","<PackMan$Dir>.Resources",
					"Choices:PackMan");

				// Ensure that default paths are present, unless they
				// have been explicitly disabled by the user.
				bool paths_changed=_package_base->paths().ensure_defaults();
				if (paths_changed) _package_base->paths().commit();

			}
		} catch(...)
		{
			// Just delete as we will be shown the create package dialog
			delete _package_base;
			_package_base=0;
		}
	}
	return (_package_base != 0);
}

/**
 * Get cached list of unique package names.
 *
 * The list is different from the order in the binary control
 * table as the names are put in case insensitive sort order
 *
 * This is calculate the first time it is requested and
 * then again after reset_package_list is called
 */
const std::vector<std::string> &Packages::package_list()
{
	if (_package_list.empty())
	{
		const pkg::binary_control_table& ctrltab = _package_base->control();
		std::string prev_pkgname = "";

		for (pkg::binary_control_table::const_iterator i=ctrltab.begin();
			 i !=ctrltab.end(); ++i)
		{
			  std::string pkgname=i->first.pkgname;
			  if (pkgname!=prev_pkgname)
			  {
				  prev_pkgname=pkgname;
				  int i = (int)_package_list.size() - 1;
				  while (i >= 0 && tbx::compare_ignore_case(pkgname, _package_list[i]) < 0) i--;
				  if (i < (int)_package_list.size() - 1)
				  {
					  _package_list.insert(_package_list.begin() + i + 1, pkgname);
				  } else
				  {
					  _package_list.push_back(pkgname);
				  }
			  }
		}
	}

	return _package_list;
}

/**
 * Reset cached list of package names
 */
void Packages::reset_package_list()
{
	_package_list.clear();
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

/**
 * Clear selection state.
 *
 * This resets the selection state to match the base state so nothing
 * is waiting for a commit.
 */
void Packages::clear_selection()
{
	pkg::status_table & seltable = _package_base->selstat ();

	pkg::status_table::const_iterator i;
	std::set < std::string > selected;

	// Get list of packages with there state and clear any old selection
	for (i = seltable.begin(); i != seltable.end(); ++i)
	{
		pkg::status curstat = _package_base->curstat ()[i->first];
		if (curstat != i->second)
		{
			selected.insert(i->first);
		}
	}

	for (std::set<std::string>::const_iterator reseti = selected.begin();
			reseti != selected.end(); ++reseti)
	{
		 pkg::status curstat = _package_base->curstat ()[*reseti];
		 seltable.insert(*reseti, curstat);
	}

	_package_base->fix_dependencies(selected);
	_package_base->remove_auto();
}

/**
 * Select a package for install or upgrade
 *
 * @param bctrl binary control record of package to install/upgrade
 */
void Packages::select_install(const pkg::binary_control *bctrl)
{
	const std::string &pkgname = bctrl->pkgname();
	std::set<std::string> seed;
	pkg::status curstat = _package_base->curstat()[pkgname];
	pkg::status selstat = _package_base->selstat()[pkgname];
	selstat.state(pkg::status::state_installed);
	const pkg::control& ctrl=_package_base->control()[pkgname];
	selstat.version(ctrl.version());
	_package_base->selstat().insert(pkgname,selstat);
	seed.insert(pkgname);
	if (!_package_base->fix_dependencies(seed))
	{
		check_dependencies(ctrl);
	}
	_package_base->remove_auto();
}

/**
 * Add latest version of given packages to selections for install or upgrade
 *
 * @param add_packages list of packages to select
 */
void Packages::select_install(const std::vector<std::string> &add_packages)
{
	std::set<std::string> seed;
	for(std::vector<std::string>::const_iterator i = add_packages.begin(); i != add_packages.end(); ++i)
	{
		std::string pkgname = *i;
		pkg::status curstat = _package_base->curstat()[pkgname];
		pkg::status selstat = _package_base->selstat()[pkgname];
		selstat.state(pkg::status::state_installed);
		const pkg::control& ctrl=_package_base->control()[pkgname];
		selstat.version(ctrl.version());
		_package_base->selstat().insert(pkgname,selstat);
		seed.insert(pkgname);
	}
	_package_base->fix_dependencies(seed);
	_package_base->remove_auto();
}

/**
 * Select a package to remove
 *
 * @param bctrl binary control record of package to removee
 */
void Packages::select_remove(const pkg::binary_control *bctrl)
{
	const std::string &pkgname = bctrl->pkgname();
	std::set<std::string> seed;
	pkg::status curstat = _package_base->curstat()[pkgname];
	pkg::status selstat = _package_base->selstat()[pkgname];
	selstat.state(pkg::status::state_removed);
	_package_base->selstat().insert(pkgname,selstat);
	seed.insert(pkgname);
	_package_base->fix_dependencies(seed);
	_package_base->remove_auto();
}

/**
 * Select all packages that can be upgraded.
 *
 * @returns true if upgrades are available
 */
bool Packages::select_upgrades()
{
	if (_upgrades_available == NO) return false;

	pkg::status_table &seltable = _package_base->selstat ();
	pkg::status_table &curtable = _package_base->curstat();
	std::set < std::string > seed;

    // Select all upgrades
    const pkg::binary_control_table& ctrltab = _package_base->control();
    std::string prev_pkgname;

    for (pkg::binary_control_table::const_iterator i=ctrltab.begin();
	 i !=ctrltab.end(); ++i)
    {
	   std::string pkgname=i->first.pkgname;
	   if (pkgname!=prev_pkgname)
	   {
		  // Don't use i->second for ctrl as it may not be the latest version
		  // instead look it up.
		  prev_pkgname=pkgname;

          pkg::status curstat=curtable[pkgname];
		  pkg::status selstat=seltable[pkgname];
		  if (selstat.state()>=pkg::status::state_installed)
		  {
		 	 const pkg::control& ctrl=ctrltab[pkgname];
			 selstat.version(ctrl.version());
			 if (_upgrades_available != YES)
			 {
				  pkg::version inst_version(curstat.version());
				  pkg::version cur_version(ctrl.version());

				  if (inst_version < cur_version)
				  {
					  _upgrades_available = YES;
				  }
			 }
			 if (_upgrades_available == YES)
			 {
				 _package_base->selstat().insert(pkgname,selstat);
				 seed.insert(pkgname);
			 }
		  }
	   }
    }

	_package_base->fix_dependencies (seed);
	_package_base->remove_auto ();

	return (_upgrades_available == YES);
}

/**
 * Remove package from the selection
 *
 * @param pkgname name of package to remove
 */
void Packages::deselect(const std::string &pkgname)
{
	std::set<std::string> seed;
	pkg::status curstat = _package_base->curstat()[pkgname];
	pkg::status selstat = _package_base->selstat()[pkgname];

	if (selstat != curstat)
	{
		selstat = curstat;
		_package_base->selstat().insert(pkgname,selstat);
		seed.insert(pkgname);
		_package_base->fix_dependencies(seed);
		_package_base->remove_auto();
	}
}

/**
 * Unset upgrades available so they will be recalculated
 */
void Packages::unset_upgrades_available()
{
	_upgrades_available = DONT_KNOW;
}

/**
 * Check if there are any upgrades available for installed packages
 */
bool Packages::upgrades_available()
{
	if (_upgrades_available == DONT_KNOW)
	{
		_upgrades_available = NO;

		const pkg::binary_control_table& ctrltab = _package_base->control();
		std::string prev_pkgname;

		for (pkg::binary_control_table::const_iterator i=ctrltab.begin();
		 i !=ctrltab.end(); ++i)
		{
		   std::string pkgname=i->first.pkgname;
		   if (pkgname!=prev_pkgname)
		   {
			  // Don't use i->second for ctrl as it may not be the latest version
			  // instead look it up.
			  prev_pkgname=pkgname;

			  pkg::status curstat=_package_base->curstat()[pkgname];
			  if (curstat.state()>=pkg::status::state_installed)
			  {
			 	  const pkg::control& ctrl=_package_base->control()[pkgname];
				  pkg::version inst_version(curstat.version());
				  pkg::version cur_version(ctrl.version());

				  if (inst_version < cur_version)
				  {
					  _upgrades_available = YES;
					  break; // Don't need to check any more
				  }
			  }
		   }
		}
	}

	return (_upgrades_available != NO);
}



/**
 * Convert paths to path relative to Boot$Dir if possible
 *
 * @param full_path path to convert
 * @returns path definition relative to boot if possible
 *          or original path if not.
 */
std::string Packages::make_path_definition(const std::string &full_path)
{
	std::string result(full_path);

	if (tbx::find_ignore_case(full_path, ".!BOOT.") == std::string::npos)
	{
		const char *boot_path = getenv("Boot$Dir");
		if (boot_path != 0 && strlen(boot_path) < result.size())
		{
			const char *parent_end = strrchr(boot_path, '.');
			if (parent_end != 0)
			{
				int match = 0;

				while (boot_path < parent_end && tolower(*boot_path) == tolower(result[match]))
				{
					boot_path++;
					match++;
				}
				if (boot_path == parent_end && result[match] == '.')
				{
					result.replace(0, match, "<Boot$Dir>.^");
				}
			}
		}
	}

	return result;
}

/**
 * Start a new log and return it
 */
std::tr1::shared_ptr<pkg::log> Packages::new_log()
{
    _log = std::tr1::shared_ptr<pkg::log>(new pkg::log());
    return _log;
}

/**
 * Get recommendations and suggestions for a list of packages
 *
 * @param packages array of package name/package version specifying packages to check
 * @param recommends vector to hold list of recommendations
 * @param suggests vector to hold list of suggestions
 */
void Packages::get_recommendations(const std::vector< std::pair<std::string, std::string> > &packages, std::vector<std::string> &recommends,  std::vector<std::string> &suggests)
{
	const pkg::binary_control_table& ctrltab = _package_base->control();
	const pkg::status_table &seltable = _package_base->selstat();
	const pkg::status_table &curtable = _package_base->curstat();
	std::set< std::string > recs_found;
	std::set< std::string > sugs_found;

	for(std::vector< std::pair<std::string, std::string> >::const_iterator i = packages.begin();
			i != packages.end(); ++i)
	{
		try
		{
			pkg::binary_control_table::key_type key(i->first, pkg::version(i->second));
			const pkg::binary_control &ctrl = ctrltab[key];
			if (ctrl.pkgname() == i->first) // double check package was found
			{
				std::string recommends = ctrl.recommends();
				if (!recommends.empty())
				{
					std::vector< std::vector<pkg::dependency> > choices;
					pkg::parse_dependency_list(recommends.begin(), recommends.end(), &choices);
					std::vector< std::vector<pkg::dependency> >::iterator list;
					for (list = choices.begin(); list != choices.end(); ++list)
					{
						for (std::vector<pkg::dependency>::iterator dep = list->begin(); dep != list->end(); ++dep)
						{
							std::set< std::string > :: iterator found = recs_found.find(dep->pkgname());
							// Add it to list or update version if it's an older version
							if (found == recs_found.end())
							{
								recs_found.insert(dep->pkgname());
							}
							found = sugs_found.find(dep->pkgname());
							if (found != sugs_found.end()) sugs_found.erase(dep->pkgname());
						}
					}
				}

				std::string suggests = ctrl.suggests();
				if (!suggests.empty())
				{
					std::vector< std::vector<pkg::dependency> > choices;
					pkg::parse_dependency_list(suggests.begin(), suggests.end(), &choices);
					std::vector< std::vector<pkg::dependency> >::iterator list;
					for (list = choices.begin(); list != choices.end(); ++list)
					{
						for (std::vector<pkg::dependency>::iterator dep = list->begin(); dep != list->end(); ++dep)
						{
							std::set< std::string >::iterator found = sugs_found.find(dep->pkgname());
							std::set< std::string >::iterator rec_found = recs_found.find(dep->pkgname());
							// Add it to list or update version if it's an older version
							if (found == sugs_found.end() && rec_found == recs_found.end())
							{
								sugs_found.insert(dep->pkgname());
							}
						}
					}
				}
			}
		} catch(...)
		{
			// Ignore any errors and just don't add the problem items
		}
	}

	for (std::set< std::string >::iterator i = recs_found.begin(); i != recs_found.end();++i)
	{
		const std::string &pkgname = *i;
		const pkg::binary_control &ctrl = ctrltab[pkgname];
		if (ctrl.pkgname() == pkgname
			&& curtable[pkgname].state() != pkg::status::state_installed
			&& seltable[pkgname].state() != pkg::status::state_installed
			)
		{
			// version not specified
			recommends.push_back(pkgname);
		}
	}

	for (std::set< std::string >::iterator i = sugs_found.begin(); i != sugs_found.end();++i)
	{
		const std::string &pkgname = *i;
		const pkg::binary_control &ctrl = ctrltab[pkgname];
		if (ctrl.pkgname() == pkgname
			&& curtable[pkgname].state() != pkg::status::state_installed
			&& seltable[pkgname].state() != pkg::status::state_installed
			)
		{
			// version not specified
			suggests.push_back(pkgname);
		}
	}
}


/**
 * Get binary control record for a named package
 *
 * @param pkgname name of package to search for
 * @returns pointer to binary_control record if found or 0 if not found
 */
const pkg::binary_control *Packages::find_control(const std::string &pkgname)
{
    const pkg::binary_control_table& ctrltab = _package_base->control();
    const pkg::binary_control &ctrl = ctrltab[pkgname];

    return (ctrl.pkgname() == pkgname) ? &ctrl : 0;
}


/**
 * Global function to format the description to show in a text area
 */
std::string format_description(const pkg::binary_control *bctrl)
{
	std::string desc = bctrl->description();
	std::string::size_type lf_pos = desc.find('\n');
	if (lf_pos != std::string::npos)
	{
		// First line is summary - so leave as is
		// Replace other line feeds with spaces except when doubled which
		// marks an end of paragraph
		while ((lf_pos = desc.find('\n', lf_pos+1))!= std::string::npos)
		{
			if (lf_pos < desc.size()-2 && desc[lf_pos+1] == '\n')
			{
				lf_pos++;
			} else
			{
				desc[lf_pos] = ' ';
			}
		}
	}

	return desc;
}

/**
 * Simple class to show a message on the next idle poll to give
 * other things a change to display
 */
class DelayMsg : tbx::Command
{
	std::string _message;
	std::string _title;
	DelayMsg(const std::string &msg, const std::string &title) :
		_message(msg),
		_title(title)
	{
		tbx::app()->add_idle_command(this);
	}
public:
	static void show(const std::string &msg, const std::string &title)
	{
		new DelayMsg(msg, title);
	}
	virtual ~DelayMsg() {}

	virtual void execute()
	{
		tbx::app()->remove_idle_command(this);
		tbx::show_message(_message, _title, "warning");
		delete this;
	}
};

/**
 * Check that the dependencies are available and show a warning
 * if they are not.
 *
 * @param depends list of dependencies string
 */
void Packages::check_dependencies(const pkg::control& ctrl)
{
	std::string missing;
	std::set<std::string> done;
	check_dependencies(ctrl.pkgname(), ctrl.depends(), missing, done);
	if (!missing.empty())
	{
		missing = "Warning: The following dependencies are not available\n" + missing;
		missing += "\n\nPlease add them before installing this package.";
		std::string title("Missing dependencies for ");
		title+=ctrl.pkgname();
		DelayMsg::show(missing, title);
	}
}

/**
 * Recursive check of dependencies
 *
 * @param pkgname package to check
 * @param depends dependencies from the package
 * @param missing text update with missing dependencies
 * @param done set of package/versions that have been checked
 */
void Packages::check_dependencies(const std::string &pkgname, const std::string &depends, std::string &missing, std::set<std::string> &done)
{
	if (depends.empty()) return;

	pkg::pkgbase *package_base = Packages::instance()->package_base();
	pkg::binary_control_table control_table = package_base->control();
	std::vector<std::vector<pkg::dependency> > depends_list;
	pkg::parse_dependency_list(depends.begin(), depends.end(), &depends_list);
	std::vector<std::vector<pkg::dependency> >::const_iterator dli;
	std::vector<pkg::dependency>::const_iterator di;
	for(dli = depends_list.begin(); dli != depends_list.end(); ++dli)
	{
		for(di = dli->begin(); di != dli->end(); ++di)
		{
			if (done.find(di->pkgname()) == done.end())
			{
				done.insert(di->pkgname());
				const pkg::binary_control &dctrl = control_table[di->pkgname()];
				if (dctrl.pkgname() != di->pkgname())
				{
					if (!missing.empty()) missing += ", ";
					missing += di->pkgname();
					missing += " for " + pkgname;
				} else if (!di->matches(dctrl.pkgname(),pkg::version(dctrl.version())))
				{
					if (!missing.empty()) missing += ", ";
					missing += di->pkgname() + "version " + di->version().package_version();
					missing += " for " + pkgname;
				} else
				{
					check_dependencies(di->pkgname(), dctrl.depends(), missing, done);
				}
			}
		}
	}
}
