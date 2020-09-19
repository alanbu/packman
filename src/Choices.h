/*********************************************************************
* This file is part of PackMan
*
* Copyright 2014-2020 AlanBuckley
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
 * Choices.h
 *
 *  Created on: 1 Aug 2014
 *      Author: alanb
 */

#ifndef CHOICES_H_
#define CHOICES_H_

#include "tbx/bbox.h"
#include <string>
#include <set>

/**
 * Class to hold choices for packman
 */
class Choices
{
	int _update_prompt_days;
	bool _enable_logging;
	tbx::BBox _main_window_pos;
	bool _small_summary_bar;;
	bool _modified;
	std::set<std::string> _override_environment;
	std::set<std::string> _override_modules;

public:
	Choices();
	virtual ~Choices();

	void load();
	bool save();

	bool modified() const {return _modified;}

	int update_prompt_days() const {return _update_prompt_days;}
	void update_prompt_days(int value);

	bool enable_logging() const {return _enable_logging;}
	void enable_logging(bool enable);

	const tbx::BBox &main_window_pos() const {return _main_window_pos;}
	void main_window_pos(const tbx::BBox &new_pos);

	bool small_summary_bar() const {return _small_summary_bar;}
	void small_summary_bar(bool small);

	const std::set<std::string> &override_environment() const {return _override_environment;}
	void override_environment(const std::set<std::string> &envs);

	const std::set<std::string> &override_modules() const {return _override_modules;}
	void override_modules(const std::set<std::string> &modules);


	static bool ensure_choices_dir();
};

/**
 * Get global choices
 */
Choices &choices();

// Location of choices directory for application
extern const char *ChoicesDir;

/**
 * Return the path to write a file in the choices directory
 */
inline std::string choices_write_path(const char *leaf_name)
{
	std::string path(ChoicesDir);
	path += ".";
	path += leaf_name;
	return path;
}

/**
 * Return the path to read a file in the choices directory
 */
inline std::string choices_read_path(const char *leaf_name)
{
	std::string path("Choices:PackMan.");
	path += leaf_name;
	return path;
}



#endif /* CHOICES_H_ */
