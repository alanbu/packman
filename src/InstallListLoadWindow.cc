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
 * InstallListLoadWindow.cc
 *
 *  Created on: 16 Jul 2015
 *      Author: alanb
 */

#include "InstallListLoadWindow.h"
#include "AgendaWindow.h"
#include "PackageConfigWindow.h"
#include "MapDrivesWindow.h"
#include "Packages.h"
#include "SourcesFile.h"
#include "libpkg/pkgbase.h"

#include "tbx/hourglass.h"
#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/messagewindow.h"
#include "tbx/questionwindow.h"
#include "tbx/stringutils.h"
#include "tbx/actionbutton.h"
#include "tbx/optionbutton.h"
#include "tbx/messagewindow.h"
#include "tbx/path.h"

/**
 * Construct and show install list window
 *
 * @param filename the install list file to read the details from
 */
InstallListLoadWindow::InstallListLoadWindow(std::string filename) :
   _show_agenda(this, &InstallListLoadWindow::show_agenda),
   _ok_command(this, &InstallListLoadWindow::ok),
   _agenda(nullptr),
   _adding_items(false),
   _add_paths(false),
   _add_sources(false),
   _update_lists_command(this, &InstallListLoadWindow::update_lists),
   _add_packages_command(this, &InstallListLoadWindow::add_packages)
{
	tbx::Hourglass hg;
	try
	{
		_list.load(filename);
		_list.remove_existing();

		hg.off();
		tbx::Window window("InstListLd");
		_window = window;
		tbx::DisplayField packages = window.gadget(3);
		tbx::DisplayField unavailable = window.gadget(10);
		tbx::DisplayField paths = window.gadget(5);
		tbx::DisplayField clash = window.gadget(12);
		tbx::DisplayField sources = window.gadget(7);
		tbx::ActionButton agenda = window.gadget(8);
		tbx::ActionButton cont = window.gadget(1);
		tbx::OptionButton add_paths = window.gadget(0xd);
		tbx::OptionButton add_sources = window.gadget(0xe);

		agenda.add_select_command(&_show_agenda);
		cont.add_select_command(&_ok_command);

		packages.text(tbx::to_string(_list.packages_count()));
		unavailable.text(tbx::to_string(_list.unavailable_packages().size()));
		paths.text(tbx::to_string(_list.paths_count()));
		clash.text(tbx::to_string(_list.clash_paths().size()));
		sources.text(tbx::to_string(_list.sources_count()));

		if (_list.paths_count())
		{
			add_paths.on(true);
			add_paths.fade(false);
		}
		if (_list.sources_count())
		{
			add_sources.on(true);
			add_sources.fade(false);
		}

		if (_list.packages().empty() && _list.unavailable_packages().empty()
				&& _list.paths().empty() && _list.sources().empty())
		{
			cont.fade(true);
		}
	} catch(std::exception &e)
	{
		hg.off();
		std::string msg("Unable to load package list!\n\nError: ");
		msg += e.what();
		tbx::show_message(msg);
	}
}

/**
 * Destructor - remove agenda window if used
 */
InstallListLoadWindow::~InstallListLoadWindow()
{
	delete _agenda;
}

/**
 * Window has been window so get rid of the agenda window if it
 * has been used.
 */
void InstallListLoadWindow::has_been_hidden(const tbx::EventInfo &event_info)
{
	if (!_adding_items)
	{
		_window.delete_object();
		delete this;
	}
}

/**
 * Show what will be done if continue is clicked
 */
void InstallListLoadWindow::show_agenda()
{
	if (_agenda == nullptr) _agenda = new AgendaWindow();
	_agenda->show(_list);
}

/**
 * Check parameters and add packages/paths/sources
 * as shown in the agenda
 */
void InstallListLoadWindow::ok()
{
	tbx::OptionButton add_paths_opt = _window.gadget(0xd);
	tbx::OptionButton add_sources_opt = _window.gadget(0xe);
	_add_paths = add_paths_opt.on();
	_add_sources = add_sources_opt.on();

	_adding_items = true;
	if (_add_paths)
	{
		std::set<std::string> drives;
		_list.get_path_drives(drives);
		if (!check_drives(drives))
		{
			show_map_drives(drives);
		} else
		{
			add_paths();
		}
	} else
	{
		check_for_list_update();
	}
}

/**
 * Reshow the window after cancelling from a window
 * shown in OK processing
 */
void InstallListLoadWindow::reshow()
{
	_window.show();
	_adding_items = false;
}

/**
 * Check if the list of given drives is accessable
 *
 * @param drives list of drives to check - updated to bad drives
 * @returns true if all drives are accessible
 */
bool InstallListLoadWindow::check_drives(std::set<std::string> &drives)
{
	if (!drives.empty())
	{
		std::set<std::string> bad_drives;
		for(auto drive : drives)
		{
			if (!check_drive(drive))
			{
				bad_drives.insert(drive);
			}
		}
		drives = bad_drives;
		return drives.empty();
	}
	return true;
}

/**
 * Check if a drive is valid
 *
 * @param drive drive to check
 * @returns true if valid
 */
bool InstallListLoadWindow::check_drive(const std::string &drive)
{
	tbx::Path check(drive);
	check.down("$");
	try
	{
		return check.exists();
	} catch(...)
	{
		// OS error also thrown for bad drive
		return false;
	}
}

/**
 * Show the map drives window
 */
void InstallListLoadWindow::show_map_drives(std::set<std::string> &drives)
{
	_window.hide();
	new MapDrivesWindow(this, drives);
}

/**
 * Check if you wish to update lists
 */
void InstallListLoadWindow::check_for_list_update()
{
	// Add any new sources first
	if (_add_sources && _list.sources_count())
	{
		SourcesFile sources_file;
		sources_file.load();
		for (auto source : _list.sources())
		{
			int source_idx = sources_file.find(source);
			if (source_idx == -1)
			{
				sources_file.add(source, true);
			} else
			{
				// Ensure the newly added source is enabled
				sources_file.enable(source_idx, true);
			}
		}
		try
		{
			sources_file.save();
			pkg::pkgbase *pkgbase = Packages::instance()->package_base();
			pkg::source_table &sources =pkgbase->sources();
			sources.update();
		} catch (std::exception &e)
		{
			// Failed to update sources so go to add packages
	    	tbx::show_message("Failed to update sources!\n\nAdding available packages only",
	    			"Install Package List", "warning", &_add_packages_command);
	    	return;
		}
	}

	if (!_list.unavailable_packages().empty())
	{
		std::string msg("One or more packages from the install list\n"
				        "are not in your current package lists.\n\n"
				        "Do you want to update the lists before proceeding?\n\n"
				        "(if you answer 'No' only the packages that are\n"
				        "available will be selected for installation)"
				);
		tbx::show_question(msg, "Update Lists?",
				&_update_lists_command,
				&_add_packages_command);
	} else
	{
		add_packages();
	}
}

/**
 * Remap drives in paths from the given table
 *
 * @param table of mappings for the paths
 */
void InstallListLoadWindow::remap_paths(std::vector<std::pair<std::string,std::string>> &mapping)
{
	_list.remap_paths(mapping);
	add_paths();
}

/**
 * Add any new paths from the list
 */
void InstallListLoadWindow::add_paths()
{
	pkg::path_table &paths = Packages::instance()->package_base()->paths();
	if (!_list.paths().empty())
	{
		for (auto path : _list.paths())
		{
			std::string path_defn = Packages::make_path_definition(path.second);
			paths.alter(path.first, path_defn);
		}
		paths.commit();
	}
	check_for_list_update();
}

/**
 * Add new packages to the component configure window
 */
void InstallListLoadWindow::add_packages()
{
	if (!_list.packages().empty())
	{
		std::vector<std::string> add_packages;
		for(auto pack : _list.packages())
		{
			add_packages.push_back(pack.name);
		}
		Packages::instance()->select_install(add_packages);
		PackageConfigWindow::update();

		// Update component flags on the config window
		for(auto pack :_list.packages())
		{
			for (auto comp : pack.components)
			{
				PackageConfigWindow::update_component_flags(comp.logical_path, comp.flags);
			}
		}
	} else
	{
		std::string msg("There were no packages that could be installed.\n\n"
				"This may be because they couldn't be found in your\n"
				"current list of available packages.");

		tbx::show_message(msg, "Install Package List", "information");
	}
	delete this;
}

/**
 * Run the update list process
 */
void InstallListLoadWindow::update_lists()
{
    if (UpdateListWindow::showing())
    {
    	tbx::show_message("List update already in progress!\n\nSkipping updating lists",
    			"Update lists","warning", &_add_packages_command);
    } else
    {
    	UpdateListWindow *ulw = new UpdateListWindow;
    	ulw->listener(this);
    }
}

/**
 * Called when update list window finishes
 *
 * @param success true if the list were successfully updated
 */
void InstallListLoadWindow::lists_updated(bool success)
{
	auto ulw = UpdateListWindow::instance();
	if (ulw) ulw->listener(nullptr);

	if (success)
	{
		_list.recheck_unavailable();
		if (_list.unavailable_packages().empty())
		{
			add_packages();
		} else
		{
	    	tbx::show_message("Not all packages were available after the list update.\n\n"
	    			          "Adding available packages only",
	    			          "Install Package List", "warning", &_add_packages_command);
		}
	} else
	{
    	tbx::show_message("List update failed!\n\n"
    			"Adding available packages only",
    			"Install Package List", "warning", &_add_packages_command);
	}
}
