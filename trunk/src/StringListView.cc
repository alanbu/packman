/*********************************************************************
* Copyright 2009 Alan Buckley
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
