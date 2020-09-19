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
