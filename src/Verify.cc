/*********************************************************************
* This file is part of PackMan
*
* Copyright 2012-2020 AlanBuckley
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

#include "Verify.h"
#include "Packages.h"
#include "libpkg/pkgbase.h"
#include "tbx/path.h"
#include <fstream>

Verify::Verify() :
   _state(BUILD_PACKAGE_LIST),
   _cancel(false),
   _package_total(0),
   _file_total(0)
{
}

/**
 * Do the next stage of the verify
 *
 * verify is finished when state() == DONE
 */
void Verify::poll()
{
   if (_cancel)
   {
      _state = DONE;
   } else
   {
      switch(_state)
      {
          case BUILD_PACKAGE_LIST:
               build_package_list();
               if (_package_total == 0) _state = DONE;
               else _state = NEXT_PACKAGE;
               break;

         case NEXT_PACKAGE:
              if (!_failed_files.empty())
              {
                 _failed_packages.push_back(FailedPackage(_current, _failed_files));
                 _failed_files.clear();
              }

              if (_packages.empty())
              {
                 _state = DONE;
              } else
              {
                 _current = _packages.top();
                 _packages.pop();
                 _state = BUILD_FILE_LIST;
              }
              break;

          case BUILD_FILE_LIST:
              build_file_list();
              _state = VERIFY_FILES;
              break;

          case VERIFY_FILES:
              if (_files.empty())
              {
                 _state = NEXT_PACKAGE;
              } else
              {
                 std::string file_name = _files.top();
                 _files.pop();
                 if (!tbx::Path(file_name).exists())
                 {
                    _failed_files.push_back(file_name);
                 }
              }
              break;

          case DONE:
              // Finished - controller should stop polling
              break;
      }
   }
}

/**
 * Set to cancel on the next poll
 */
void Verify::cancel()
{
   _cancel = true;
}

/**
 * Build list of all installed packages to verify
 */
void Verify::build_package_list()
{
   pkg::pkgbase *package_base = Packages::instance()->package_base();
   const pkg::status_table& curstat = package_base->curstat();
   _package_total = 0;

   for (pkg::status_table::const_iterator i=curstat.begin();
	 i !=curstat.end(); ++i)
   {
		if ((*i).second.state() == pkg::status::state_installed)
		{
			 _packages.push(i->first);
			 _package_total++;
		}
   }
}

/**
 * Build list of files for the current package
 */
void Verify::build_file_list()
{
    pkg::pkgbase *package_base = Packages::instance()->package_base();
	std::string prefix=package_base->info_pathname(_current);
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
      if (!line.empty() && line.back() != '.')
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
      if (!line.empty() && line.back() != '.')
      {
		    mf.insert(line);
      }
		bak_in.peek();
	}

    _file_total = 0;
    for (std::set<std::string>::iterator i = mf.begin(); i != mf.end(); ++i)
    {
    	_files.push(paths(*i, _current));
    	_file_total++;
    }
}
