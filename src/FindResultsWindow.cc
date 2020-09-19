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
 * FindResultsWindow.cc
 *
 *  Created on: 4 Jun 2015
 *      Author: alanb
 */

#include "FindResultsWindow.h"
#include "InfoWindow.h"
#include "FileFoundWindow.h"
#include "Packages.h"
#include "libpkg/pkgbase.h"

#include "tbx/deleteonhidden.h"

/**
 * Construct the window from Finder results
 *
 * @param find_text text that was searched for
 * @param results finder results to show
 */
FindResultsWindow::FindResultsWindow(const std::string &find_text, const Finder::Results &results) :
   _find_text(find_text),
   _value_provider(this),
   _renderer(&_value_provider),
   _view(tbx::Window("FindResults"), &_renderer)
{
	tbx::Font font;
	if (font.desktop_font()) _renderer.font(font);
	_view.selection(&_selection);

	std::string last_package = "";
	for(auto result : results)
	{
		if (result.package != last_package)
		{
			last_package = result.package;
			_items.push_back(std::make_pair(true, last_package));
		}
		_items.push_back(std::make_pair(false, result.logical_path));
	}
	_view.inserted(0, _items.size());
	_view.size_to_width(_view.window().size().width);
	_view.add_click_listener(this);

	tbx::Menu menu = _view.window().menu();
	menu.add_about_to_be_shown_listener(this);
	menu.add_selection_listener(this);

	// Dump class and object when window is hidden
	_view.window().add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<FindResultsWindow>(this));
	_view.window().add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
}

FindResultsWindow::~FindResultsWindow()
{
}

/**
 * Check for double click on item to show details
 *
 * @param event details on click type and location
 */
void FindResultsWindow::itemview_clicked(const tbx::view::ItemViewClickEvent &event)
{
	if (event.item_hit() && event.click_event().is_select_double())
	{
		show_info(event.index());
	}
}

/**
 * Show information on selected item
 *
 * @param event - not used as we only have one menu item
 */
void FindResultsWindow::menu_selection(const tbx::EventInfo &event)
{
	if (!_selection.empty())
	{
		show_info(_selection.first());
	}
}

/**
 * Show information on the given indedx
 *
 * @param index of item to show information of
 */
void FindResultsWindow::show_info(unsigned int index)
{
	auto &item = _items[index];
	if (item.first)
	{
		// Package name
		InfoWindow::show(item.second);
	} else
	{
		// File
		// Find package for file
		index--;
		while (index > 0 && !_items[index].first) index--;
		new FileFoundWindow(_find_text, _items[index].second, item.second, "File details");
	}
}

/**
 * About to be shown processing for the menu (not the window)
 *
 * @param event details of the event
 */
void FindResultsWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	tbx::MenuItem menu_item = tbx::Menu(event.id_block().self_object()).item(0);
	if (!_selection.empty())
	{
		std::string name = _items[_selection.first()].second;
		std::string::size_type dot_pos = name.rfind('.');
		if (dot_pos != std::string::npos) name.erase(0, dot_pos+1);
		std::string text("Info on '");
		text += name;
		if (text.size() > 62) text.erase(62);
		text += "'";
		menu_item.text(text);
		menu_item.fade(false);
	} else
	{
		menu_item.text("Info on ''");
		menu_item.fade(true);
	}
}


/**
 * Provide text for line of view
 */
std::string FindResultsWindow::ValueProvider::value(unsigned int index) const
{
	auto item = _me->_items[index];
	std::string line;
	if (item.first)
	{
		if (_me->_view.selection()->selected(index))
		{
			line = tbx::fpcs::colour(tbx::Colour::cyan,tbx::Colour::black);
		} else
		{
			line = tbx::fpcs::colour(tbx::Colour::light_blue, tbx::Colour::white);
		}
		line += "Package: " + item.second;
	} else
	{
		pkg::pkgbase *pb = Packages::instance()->package_base();
		std::string install_path = pb->paths()(item.second, "");

		line = tbx::fpcs::move_x_os(32) + install_path;
	}
	return line;
}
