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
#include "tbx/actionbutton.h"
#include "tbx/stringset.h"

#include "ReportView.h"
#include "Commands.h"

#include "libpkg/binary_control.h"
#include "libpkg/table.h"
#include <vector>

class SummaryWindow;
class PackageFilter;

class MainWindow :
	tbx::TextChangedListener,
	pkg::table::watcher,
	tbx::SelectionListener
{
	tbx::Window _window;
	tbx::ActionButton _install_button;
	tbx::ActionButton _remove_button;
	tbx::StringSet _filters_stringset;

	ReportView _view;
	tbx::SingleSelection _selection;
	PackageFilter *_filter;

	std::vector<const pkg::binary_control *> _shown_packages;

	// Column drawing
	class StatusRenderer : public SpriteItemRenderer
	{
		MainWindow *_me;
		static tbx::UserSprite _sprites[4];

	public:
		StatusRenderer(MainWindow *me);
		virtual tbx::Sprite *sprite(unsigned int index) const;
	} _status_renderer;

	/**
	 * Class to display the package name
	 */
	class NameRenderer : public WimpFontItemRenderer
	{
		MainWindow *_me;
	public:
		NameRenderer(MainWindow *me) : _me(me) {};
		virtual std::string text(unsigned int index) const
		{
			return _me->_shown_packages[index]->pkgname();
		}
	} _name_renderer;

	/**
	 * Class to display the package summary
	 */
	class SummaryRenderer : public WimpFontItemRenderer
	{
		MainWindow *_me;
	public:
		SummaryRenderer(MainWindow *me) : _me(me) {};
		virtual std::string text(unsigned int index) const
		{
			return _me->_shown_packages[index]->short_description();
		}
	} _summary_renderer;

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

public:
	MainWindow();
	virtual ~MainWindow();

    static MainWindow *from_window(tbx::Window window);

	void refresh();

	const pkg::binary_control *selected_package();

	virtual void text_changed(tbx::TextChangedEvent &event);
	virtual void handle_change(pkg::table& t);
	virtual void selection_changed(const tbx::SelectionChangedEvent &event);

	enum InstallState {NOT_INSTALLED, INSTALLED, OLD_VERSION};

	InstallState install_state(const pkg::binary_control *bctrl, bool *auto_inst = 0);

	void search(const std::string &text, bool in_current_filter);

};

#endif /* MAINWINDOW_H_ */
