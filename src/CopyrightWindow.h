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
 * CopyrightWindow.h
 *
 *  Created on: 19-Aug-2009
 *      Author: alanb
 */

#ifndef COPYRIGHTWINDOW_H_
#define COPYRIGHTWINDOW_H_

#include "tbx/view/textview.h"
#include "libpkg/binary_control.h"

/**
 * Class to show copyright of currently selected package
 */
class CopyrightWindow
{
	tbx::view::TextView _view;

public:
	CopyrightWindow(const pkg::binary_control *ctrl);
	virtual ~CopyrightWindow();
};

#endif /* COPYRIGHTWINDOW_H_ */
