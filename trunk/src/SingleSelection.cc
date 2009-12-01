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
 * SingleSelection.cc
 *
 *  Created on: 24-Mar-2009
 *      Author: alanb
 */

#include "SingleSelection.h"
#include <algorithm>

SingleSelection::SingleSelection() : _current(SingleSelection::none),
    _listeners(0)
{
}

SingleSelection::~SingleSelection()
{
}

/**
 * Clear the selection
 */
void SingleSelection::clear()
{
	if (_current != none)
	{
		unsigned int old = _current;
		_current = none;
		fire_listener(old);
	}
}

/**
 * Select an index.
 * For a single selection this unselects the current selection
 */
void SingleSelection::select(unsigned int index)
{
	if (index != _current)
	{
		int old = _current;
		_current = index;
		fire_listener(old);
	}
}

/**
 * deselect the given index.
 * Does nothing is index is not selected.
 */
void SingleSelection::deselect(unsigned int index)
{
	if (index != _current)
	{
		int old = _current;
		_current = none;
		fire_listener(old);
	}
}

/**
 * Toggle selection state.
 * Moves selection to this location if unselected
 */
void SingleSelection::toggle(unsigned int index)
{
   int old = _current;
   if (index == _current) _current = none;
   else _current = index;
   fire_listener(old);
}

/**
 * Items have been inserted so move selection up if necessary.
 */
void SingleSelection::inserted(unsigned int index, unsigned int how_many)
{
	if (_current != none && _current >= index) _current += how_many;
}

/**
 * Items have been remove so move or delete selection
 */
void SingleSelection::removed(unsigned int index, unsigned int how_many)
{
	if (_current != none && _current >= index)
	{
		if (_current < index + how_many) _current = none;
		else _current -= how_many;
	}
}

/**
 * Add a listener to the current selection
 */
void SingleSelection::add_listener(SingleSelectionListener *listener)
{
	if (_listeners == 0) _listeners = new std::vector<SingleSelectionListener *>();
	_listeners->push_back(listener);
}

/**
 * Remove a listener to the current selection
 */
void SingleSelection::remove_listener(SingleSelectionListener *listener)
{
	if (_listeners)
	{
		std::vector<SingleSelectionListener *>::iterator found = std::find(_listeners->begin(), _listeners->end(), listener);
		if (found != _listeners->end()) _listeners->erase(found);
	}
}

/**
 * Helper to fire the listeners. Pass it the old index
 * and it uses the current item as the new index.
 */
void SingleSelection::fire_listener(unsigned int old_index)
{
	if (_listeners != 0)
	{
		unsigned int new_index = _current;
		std::vector<SingleSelectionListener *>::iterator i;
		for (i = _listeners->begin(); i != _listeners->end(); ++i)
		{
			(*i)->selection_changed(old_index, new_index);
		}
	}
}
