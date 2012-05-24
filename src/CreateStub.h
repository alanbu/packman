/*********************************************************************
* Copyright 2011 Alan Buckley
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
 * CreateStub.h
 *
 *  Created on: 11 Oct 2011
 *      Author: alanb
 */

#ifndef CREATESTUB_H_
#define CREATESTUB_H_

#include <string>
#include "tbx/path.h"

void create_application_stub(const tbx::Path &source, const std::string &target);
void create_application_link(const tbx::Path &source, const std::string &target);

#endif /* CREATESTUB_H_ */
