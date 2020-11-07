/*********************************************************************
* This file is part of PackMan
*
* Copyright 2013-2020 AlanBuckley
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

#include "FileListWindow.h"
#include "Packages.h"
#include "tbx/deleteonhidden.h"
#include "tbx/hourglass.h"
#include "libpkg/pkgbase.h"
#include "libpkg/binary_control.h"
#include <fstream>

FileListWindow::FileListWindow(const pkg::binary_control *pkg_control) :
	_window("FileListWnd"),
	_list(_window)
{
	// Put package name in title
	std::string title = _window.title();
	std::string::size_type squote_pos = title.find('\'');
	std::string::size_type equote_pos = title.find('\'', squote_pos+1);
	if (squote_pos == std::string::npos) squote_pos = title.size();
	if (equote_pos == std::string::npos) equote_pos = title.size();

	title = title.substr(0, squote_pos+1) + pkg_control->pkgname() + title.substr(equote_pos);
	_window.title(title.substr(0,127));

	tbx::Hourglass hg;
	populate_filelist(pkg_control);
	// Delete window when it has been hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<FileListWindow>(this));
}

FileListWindow::~FileListWindow()
{
	_window.delete_object();
}

/**
 * Populate the list of installed files for the given package
 */
void FileListWindow::populate_filelist(const pkg::binary_control *pkg_control)
{
    pkg::pkgbase *package_base = Packages::instance()->package_base();
	std::string pkgname(pkg_control->pkgname());
	std::string prefix=package_base->info_pathname(pkgname);
	std::string files_pathname=prefix + ".Files";
	std::string files_bak_pathname=prefix + ".Files--";
    pkg::path_table &paths = Packages::instance()->package_base()->paths();
    std::set<std::string> mf;

	// Get files from manifest that match or are children of the logical path
	std::ifstream dst_in(files_pathname.c_str());
	dst_in.peek();
	while (dst_in&&!dst_in.eof())
	{
		std::string line;
		std::getline(dst_in,line);
      // Only validating files (not directories)
      if (!line.empty() && line.back() != '.' && line.compare(0,8,"RiscPkg.") != 0)
      {
		   mf.insert(line);
      }
		dst_in.peek();
	}

	// Add in from backup file if it exists
	std::ifstream bak_in(files_bak_pathname.c_str());
	bak_in.peek();
	while (bak_in&&!bak_in.eof())
	{
		std::string line;
		std::getline(bak_in,line);
      // Only validating files (not directories)
      if (!line.empty() && line.back() != '.' && line.compare(0,8,"RiscPkg.") != 0)
      {
		    mf.insert(line);
      }
		bak_in.peek();
	}

   std::vector<std::string> files;
	for (std::set<std::string>::iterator i = mf.begin(); i != mf.end(); ++i)
    {
    	files.push_back(paths(*i, pkgname));
    }

	_list.assign(files.begin(), files.end());
}
