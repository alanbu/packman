/*********************************************************************
* Copyright 2014 Alan Buckley
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
 * Choices.cc
 *
 *  Created on: 1 Aug 2014
 *      Author: alanb
 */

#include "Choices.h"
#include "tbx/propertyset.h"

static Choices g_choices;
Choices &choices() {return g_choices;}

const int DEFAULT_PROMPT_DAYS = 7;


Choices::Choices() :
		_update_prompt_days(DEFAULT_PROMPT_DAYS)
{

}

Choices::~Choices()
{
}

void Choices::update_prompt_days(int value)
{
	if (value != _update_prompt_days)
	{
		_update_prompt_days = value;
		_modified = true;
	}
}

/**
 * Load choices file if it exists
 */
void Choices::load()
{
	tbx::PropertySet ps;
	if (ps.load("Choices:PackMan.Choices"))
	{
		_update_prompt_days = ps.get("UpdatePromptDays", DEFAULT_PROMPT_DAYS);
	}
	_modified = false;
}

/**
 * Save the choices
 *
 * @return true if choices saved
 */
bool Choices::save()
{
	tbx::PropertySet ps;
	ps.set("UpdatePromptDays", _update_prompt_days);
	if (ps.save("<Choices$Write>.PackMan.Choices"))
	{
		_modified = false;
		return true;
	}
	return false;
}
