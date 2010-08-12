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
 * MainWindow.h
 *
 *  Created on: 20-Mar-2009
 *      Author: alanb
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "tbx/window.h"
#include "tbx/sprite.h"
#include "tbx/textchangedlistener.h"
#include "tbx/toolaction.h"
#include "tbx/stringset.h"

#include "tbx/view/reportview.h"
#include "Commands.h"

#include "libpkg/binary_control.h"
#include "libpkg/table.h"
#include <vector>

class SummaryWindow;
class PackageFilter;

class MainWindow :
	tbx::TextChangedListener,
	pkg::table::watcher,
	tbx::view::SelectionListener
{
	tbx::Window _window;
	tbx::ToolAction _install_button;
	tbx::ToolAction _remove_button;
	tbx::ToolAction _apps_button;
	tbx::ToolAction _info_button;
	tbx::StringSet _filters_stringset;

	tbx::view::ReportView _view;
	tbx::view::SingleSelection _selection;
	PackageFilter *_filter;

	std::vector<const pkg::binary_control *> _shown_packages;

	// Column drawing - classes to get data for columns
	class StatusSprite : public tbx::view::ItemViewValue<tbx::Sprite *>
	{
		MainWindow *_me;
		static tbx::UserSprite _sprites[4];

	public:
		StatusSprite(MainWindow *me);
		virtual tbx::Sprite *value(unsigned int index) const;
	} _status_sprite;

	/**
	 * Class to get the package name
	 */
	class NameText : public tbx::view::ItemViewValue<std::string>
	{
		MainWindow *_me;
	public:
		NameText(MainWindow *me) : _me(me) {};
		virtual std::string value(unsigned int index) const
		{
			return _me->_shown_packages[index]->pkgname();
		}
	} _name_text;

	/**
	 * Class to get the package summary
	 */
	class SummaryText : public tbx::view::ItemViewValue<std::string>
	{
		MainWindow *_me;
	public:
		SummaryText(MainWindow *me) : _me(me) {};
		virtual std::string value(unsigned int index) const
		{
			return _me->_shown_packages[index]->short_description();
		}
	} _summary_text;

	// Column renderers
	tbx::view::SpriteItemRenderer _status_renderer;
	tbx::view::WimpFontItemRenderer _name_renderer;
	tbx::view::WimpFontItemRenderer _summary_renderer;

	/******************************************
	 * Commands
	 *****************************************/

	InstallCommand _install;
	RemoveCommand _remove;

	/********************************************
	 * Toolbars
	 */
	SummaryWindow *_summary;

	// Search information
	PackageFilter *_search_filter;

	// Listener to remove menu selection
	class StoreMenuSelection : public tbx::HasBeenHiddenListener
	{
		tbx::view::ItemView *_view;

	public:
		StoreMenuSelection(tbx::view::ItemView *view) : _view(view)
		{
			menu_selection = tbx::view::ItemView::NO_INDEX;
		}

		/**
		 * Last item selected when mouse menu button was clicked.
		 * This is updated when the menu is hidden
		 */
		unsigned int menu_selection;
		/**
		 * Clears the menu selection when the object is hidden
		 */
		virtual void has_been_hidden(const tbx::EventInfo &hidden_event)
		{
			menu_selection = (_view->last_selection_menu()) ?
					_view->selection()->first() : tbx::view::ItemView::NO_INDEX;
			_view->clear_menu_selection();
		}
	} _store_menu_select;

public:
	MainWindow();
	virtual ~MainWindow();

    static MainWindow *from_window(tbx::Window window);

	void refresh();

	const pkg::binary_control *selected_package();

	virtual void text_changed(tbx::TextChangedEvent &event);
	virtual void handle_change(pkg::table& t);
	virtual void selection_changed(const tbx::view::SelectionChangedEvent &event);

	enum InstallState {NOT_INSTALLED, INSTALLED, OLD_VERSION};

	InstallState install_state(const pkg::binary_control *bctrl, bool *auto_inst = 0);

	void search(const std::string &text, bool in_current_filter);

	void summary_size_changed(int by);
};

#endif /* MAINWINDOW_H_ */
