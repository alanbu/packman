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

#ifndef VERFIYFAILEDWINDOW_H_
#define VERFIYFAILEDWINDOW_H_

#include "Verify.h"

#include "tbx/window.h"
#include "tbx/command.h"
#include <set>
#include <vector>

/**
 * Class to give a graphical way to handle conflicts
 * reported by the commit process
 */
class VerifyFailedWindow
{
	tbx::Window _window;
	tbx::CommandMethod<VerifyFailedWindow> _show_files;
	tbx::CommandMethod<VerifyFailedWindow> _how_to_fix;

	std::vector<std::string> _missing_files;

public:
	VerifyFailedWindow(const std::vector<Verify::FailedPackage> &failures);
	virtual ~VerifyFailedWindow();

	void show_files();
	void how_to_fix();
};

#endif /* VERFIYFAILEDWINDOW_H_ */
