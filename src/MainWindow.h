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
#include "tbx/deleteonhidden.h"

#include "tbx/view/reportview.h"
#include "Commands.h"

#include "libpkg/binary_control.h"
#include "libpkg/table.h"
#include <vector>

class SummaryWindow;
class PackageFilter;

// Debug Flag to help track down if MainWindow is being used
// when deleted or overwritten
#define MAGIC_CHECK

/**
 * Class to handle the main window which shows packages and
 * lets them be maintained.
 */
class MainWindow :
	public ISelectedPackage,
	tbx::TextChangedListener,
	pkg::table::watcher,
	tbx::view::SelectionListener
{
#ifdef MAGIC_CHECK
	unsigned int _magic;
#endif
	tbx::Window _window;
	tbx::ToolAction _install_button;
	tbx::ToolAction _remove_button;
	tbx::ToolAction _apps_button;
	tbx::ToolAction _info_button;
	tbx::StringSet _status_filter_stringset;
	tbx::StringSet _section_filter_stringset;

	tbx::view::ReportView _view;
	tbx::view::SingleSelection _selection;
	PackageFilter *_status_filter;
	PackageFilter *_section_filter;

	std::vector<const pkg::binary_control *> _shown_packages;

	// Column drawing - classes to get data for columns
	class StatusSprite : public tbx::view::ItemViewValue<tbx::Sprite *>
	{
		MainWindow *_me;
		static tbx::UserSprite _sprites[8];

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

	// Class to draw environment sprite
	class EnvSprite : public tbx::view::ItemViewValue<tbx::Sprite *>
	{
		MainWindow *_me;
		static tbx::UserSprite _unset;
		static tbx::UserSprite _unknown;
		static tbx::UserSprite _invalid;
	public:
		EnvSprite(MainWindow *me);
		virtual tbx::Sprite *value(unsigned int index) const;
	} _env_sprite;

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
	tbx::view::SpriteItemRenderer _env_renderer;
	tbx::view::WimpFontItemRenderer _summary_renderer;

	/******************************************
	 * Commands
	 *****************************************/

	InstallCommand _install;
	RemoveCommand _remove;
	ShowInfoCommand _show_info;
	CopyrightCommand _copyright;
	tbx::CommandMethod<MainWindow> _save_pos;

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

	class ShowInfoOnDblClick : public tbx::view::ItemViewClickListener
	{
		MainWindow *_main;
	public:
		ShowInfoOnDblClick(MainWindow *main) {_main = main;}
		virtual ~ShowInfoOnDblClick() {}
		virtual void itemview_clicked(const tbx::view::ItemViewClickEvent &event);

	} _show_info_on_dblclick;
public:
	MainWindow();
	virtual ~MainWindow();

    static MainWindow *from_window(tbx::Window window);

	void refresh();

	const pkg::binary_control *selected_package();

	virtual void text_changed(tbx::TextChangedEvent &event);
	virtual void handle_change(pkg::table& t);
	virtual void selection_changed(const tbx::view::SelectionChangedEvent &event);

	enum InstallState {NOT_INSTALLED, PART_INSTALLED, INSTALLED, OLD_VERSION};

	InstallState install_state(const pkg::binary_control *bctrl, bool *auto_inst = 0);

	void search(const std::string &text, bool in_current_status, bool in_current_section);
	void set_section_filter(const char *name);
	void set_status_filter(const char *name);

	void summary_size_changed(int by);

	void save_position();

private:
	void section_filter_changed(const std::string &name);
	void status_filter_changed(const std::string &name);
	void update_toolbar(int index);
};

#endif /* MAINWINDOW_H_ */
