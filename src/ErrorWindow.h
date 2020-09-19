/*********************************************************************
* This file is part of PackMan
*
* Copyright 2010-2020 AlanBuckley
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
 * ErrorWindow.h
 *
 *  Created on: 11 Feb 2010
 *      Author: alanb
 */

#ifndef ERRORWINDOW_H_
#define ERRORWINDOW_H_

#include "tbx/window.h"

/**
 * Class to display and Error in a window
 */
class ErrorWindow
{
	tbx::Window _window;

	void init(const std::string &errmsg, const std::string &title);

public:
	ErrorWindow(std::string errmsg, std::string title);
	ErrorWindow(const char *prefix, std::string errmsg, std::string title);
	virtual ~ErrorWindow();
};

#endif /* ERRORWINDOW_H_ */
