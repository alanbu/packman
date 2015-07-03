/*********************************************************************
* Copyright 2015 Alan Buckley
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
 * FindResultsWindow.h
 *
 *  Created on: 4 Jun 2015
 *      Author: alanb
 */

#ifndef FINDRESULTSWINDOW_H_
#define FINDRESULTSWINDOW_H_

#include "Finder.h"
#include "tbx/view/listview.h"
#include "tbx/view/fontitemrenderer.h"
#include "tbx/abouttobeshownlistener.h"
#include "tbx/menuselectionlistener.h"

/**
 * Class to show multiple results from the find components/installed files
 */
class FindResultsWindow : tbx::view::ItemViewClickListener,
   tbx::AboutToBeShownListener,
   tbx::MenuSelectionListener
{
	std::string _find_text;
	std::vector<std::pair<bool, std::string>> _items;

	/**
	 * Class to provide the text for display
	 */
	class ValueProvider : public tbx::view::ItemViewValue<std::string>
	{
		FindResultsWindow *_me;
	public:
		ValueProvider(FindResultsWindow *me) : _me(me) {}
	      /**
	       * Provide the value for the view
	       */
	      virtual std::string value(unsigned int index) const;
	} _value_provider;
	friend class ValueProvider;
	tbx::view::FontItemRenderer _renderer;
	tbx::view::ListView _view;
	tbx::view::SingleSelection _selection;
public:
	FindResultsWindow(const std::string &find_text, const Finder::Results &results);
	virtual ~FindResultsWindow();

protected:
	virtual void itemview_clicked(const tbx::view::ItemViewClickEvent &event);
	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void menu_selection(const tbx::EventInfo &event);
	void show_info(unsigned int index);
};

#endif /* FINDRESULTSWINDOW_H_ */
