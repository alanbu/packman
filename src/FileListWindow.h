/*********************************************************************
* This file is part of PackMan
*
* Copyright 2020 AlanBuckley
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

#ifndef FILELISTWINDOW_H_
#define FILELISTWINDOW_H_

#include "tbx/window.h"
#include "StringListView.h"
#include <vector>

namespace pkg
{
	class binary_control;
}

/**
 * Class to display the files from an installed package
 */
class FileListWindow
{
	tbx::Window _window;
	StringListView _list;

public:
	FileListWindow(const pkg::binary_control *pkg_control);
	virtual ~FileListWindow();

private:
    void populate_filelist(const pkg::binary_control *pkg_control);
};

#endif /* MISSINGFILESWINDOW_H_ */
