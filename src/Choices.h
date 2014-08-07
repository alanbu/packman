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
 * Choices.h
 *
 *  Created on: 1 Aug 2014
 *      Author: alanb
 */

#ifndef CHOICES_H_
#define CHOICES_H_

/**
 * Class to hold choices for packman
 */
class Choices
{
	int _update_prompt_days;
	bool _modified;

public:
	Choices();
	virtual ~Choices();

	void load();
	bool save();

	bool modified() const {return _modified;}

	int update_prompt_days() const {return _update_prompt_days;}
	void update_prompt_days(int value);
};

/**
 * Get global choices
 */
Choices &choices();

#endif /* CHOICES_H_ */
