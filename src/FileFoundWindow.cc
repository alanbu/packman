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
 * FileFoundWindow.cc
 *
 *  Created on: 15 May 2015
 *      Author: alanb
 */

#include "FileFoundWindow.h"
#include "Packages.h"
#include "PackageFilter.h"
#include "InfoWindow.h"

#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/actionbutton.h"
#include "tbx/button.h"
#include "tbx/deleteonhidden.h"
#include "tbx/path.h"
#include "tbx/stringutils.h"
#include "tbx/sprite.h"

#include "libpkg/status_table.h"
#include "libpkg/pkgbase.h"

#include <iomanip>
#include <cstdlib>

/**
 * Construct the file found window
 *
 * @param find_text text that was searched for
 * @param package package name where file was found
 * @param logical_path logical path of found file
 * @param title title to use for the window
 */
FileFoundWindow::FileFoundWindow(const std::string &find_text, const std::string &package, const std::string &logical_path, const std::string &title) :
    _find_text(find_text),
	_package(package),
	_logical_path(logical_path),
	_show_info(this, &FileFoundWindow::show_info)
{
	tbx::Window window("FileFound");
	window.title(title);

	tbx::DisplayField comp = window.gadget(1);
	tbx::DisplayField pack = window.gadget(3);
	tbx::DisplayField inst = window.gadget(4);
	tbx::DisplayField loc = window.gadget(6);
	tbx::DisplayField match = window.gadget(0xb);
	tbx::DisplayField on_disc = window.gadget(0x14);
	tbx::Button type_icon = window.gadget(9);
	tbx::DisplayField type_text = window.gadget(0xe);
	tbx::DisplayField size = window.gadget(0x10);
	tbx::DisplayField date = window.gadget(0x12);
	tbx::ActionButton info_button = window.gadget(8);
	info_button.add_selected_command(&_show_info);

	pkg::pkgbase *pb = Packages::instance()->package_base();
	std::string install_path = pb->paths()(_logical_path, _package);
	tbx::Path path(install_path);
	comp.text(path.leaf_name());
	loc.text(path.parent().name());

	// Default match is partial leafname, so set if it would be different
	std::string::size_type find_leaf_pos = _find_text.rfind('.');
	if (find_leaf_pos == std::string::npos)
	{
		// Leaf name only matching
		if (tbx::equals_ignore_case(_find_text, path.leaf_name()))
		{
			match.text("Leafname");
		}
	} else if (tbx::equals_ignore_case(_find_text, install_path))
	{
		match.text("Exact");
	} else if (tbx::equals_ignore_case(_find_text.substr(find_leaf_pos+1), path.leaf_name()))
	{
		match.text("Leafname");
	} else if (tbx::equals_ignore_case(_find_text.substr(0, find_leaf_pos), path.parent().name()))
	{
		match.text("Directory + partial leafname");
	}

	pack.text(_package);

	pkg::status_table::const_iterator sti = pb->curstat().find(_package);
	if (sti != pb->curstat().end()
		&& (*sti).second.state() == pkg::status::state_installed)
	{
		inst.text("Installed");
	} else
	{
		inst.text("Not installed");
	}

	tbx::PathInfo info;
	if (path.path_info(info))
	{
		on_disc.text("Yes");
		size.text(tbx::to_string(info.length()));
		tbx::WimpSprite type_sprite(info.file_type(), path.leaf_name());
		if (type_sprite.exist()) type_icon.value(type_sprite.name());
		std::ostringstream ftss;
		ftss << std::hex  << std::setfill('0') << std::setw(3) << info.file_type();
		std::string fthex = ftss.str();
		std::string ftvar = "File$Type_" + fthex;
		char *ftname = std::getenv(ftvar.c_str());
		std::ostringstream ftype;
		bool add_type_hex = false;
		if (ftname)
		{
			add_type_hex = true;
			ftype << ftname;
		} else
		{
			switch(info.file_type())
			{
			case 0x1000: ftype << "Directory"; break;
			case 0x2000: ftype << "Application"; break;
			default:
				ftype << "&" << tbx::to_upper(fthex);
				add_type_hex = true;
				break;
			}
		}
		if (add_type_hex) ftype << "  (" << fthex << ")";
		type_text.text(ftype.str());
		date.text(info.modified_time().text());

		size.fade(false);
		type_text.fade(false);
		date.fade(false);
	}

	// Dump class and object when window is hidden
	window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<FileFoundWindow>(this));
	window.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
}

FileFoundWindow::~FileFoundWindow()
{
}

/**
 * Show information on the package
 */
void FileFoundWindow::show_info()
{
	InfoWindow::show(_package);
}
