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
 * StringListView.cc
 *
 *  Created on: 28-Jul-2009
 *      Author: alanb
 */

#include "StringListView.h"

StringListView::StringListView(tbx::Window window) :
	_item_value(_lines),
	_line_renderer(&_item_value),
	_view(window, &_line_renderer)

{
}

StringListView::~StringListView()
{
}

/**
 * Clear the whole list
 */
void StringListView::clear()
{
	_lines.clear();
	_view.cleared();
}

/**
 * Add line to end of view
 */
void StringListView::push_back(std::string value)
{
	_lines.push_back(value);
	_view.inserted(_lines.size()-1, 1);
}

/**
 * Remove line from end of view
 */
void StringListView::pop_back()
{
	_view.removing(_lines.size()-1,1);
	_lines.pop_back();
	_view.removed(_lines.size(), 1);
}

/**
 * Replace a line
 */
void StringListView::line(int index, std::string new_value)
{
	_view.changing(index, 1);
	_lines[index] = new_value;
	_view.changed(index, 1);
}
