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
 * SingleSelection.h
 *
 *  Created on: 24-Mar-2009
 *      Author: alanb
 */

#ifndef SINGLESELECTION_H_
#define SINGLESELECTION_H_

#include <vector>

/**
 * Listener class for selection changes
 */
class SingleSelectionListener
{
public:
	virtual ~SingleSelectionListener() {};

	virtual void selection_changed(unsigned int old_index, unsigned int new_index) = 0;
};

/**
 * Class to handle a single indexed selection
 */
class SingleSelection
{
private:
	unsigned int _current;
	std::vector<SingleSelectionListener* >*_listeners;
public:
	// Variable for no selection
	static const unsigned int none = -1;

	SingleSelection();
	virtual ~SingleSelection();

	/**
	 * Check if there is no selection
	 */
	bool empty() const {return (_current == none);}

	/**
	 * Get current selected index
	 * @returns selected index or SingleSelection::none if no item is selected
	 */
	unsigned int selected() const {return _current;}

	/**
	 * Check if an index is selection
	 */
	bool is_selected(unsigned int index) const {return (index == _current);}

	// Modify selection
	void clear();
	void select(unsigned int index);
	void deselect(unsigned int index);
	void toggle(unsigned int index);

	// Inform selection of changes to list its watching
	void inserted(unsigned int index, unsigned int how_many);
	void removed(unsigned int index, unsigned int how_many);

	// Listeners
	void add_listener(SingleSelectionListener *listener);
	void remove_listener(SingleSelectionListener *listener);

private:
	void fire_listener(unsigned int old_index);
};

#endif /* SINGLESELECTION_H_ */
