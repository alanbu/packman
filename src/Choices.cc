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
 * Choices.cc
 *
 *  Created on: 1 Aug 2014
 *      Author: alanb
 */

#include "Choices.h"
#include "tbx/propertyset.h"
#include "tbx/path.h"
#include <sstream>

static Choices g_choices;
Choices &choices() {return g_choices;}
#ifdef PKGTEST
  const char *ChoicesDir = "PkgTestDisc:!Boot.Choices.PkgTest";
#else
  const char *ChoicesDir = "<Choices$Write>.PackMan";
#endif


const int DEFAULT_PROMPT_DAYS = 7;


Choices::Choices() :
		_update_prompt_days(DEFAULT_PROMPT_DAYS),
		_enable_logging(false),
		_main_window_pos(66,184,1518,1078),
		_small_summary_bar(false),
		_modified(false)
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

void Choices::enable_logging(bool enable)
{
	if (enable != _enable_logging)
	{
		_enable_logging = enable;
		_modified = true;
	}
}

void Choices::main_window_pos(const tbx::BBox &new_pos)
{
	if (new_pos != _main_window_pos)
	{
		_main_window_pos = new_pos;
		_modified = true;
	}
}

void Choices::small_summary_bar(bool small)
{
	if (small != _small_summary_bar)
	{
		_small_summary_bar = small;
		_modified = true;
	}
}

void Choices::override_environment(const std::set<std::string> &envs)
{
	if (_override_environment != envs)
	{
		_override_environment = envs;
		_modified = true;
	}
}

void Choices::override_modules(const std::set<std::string> &modules)
{
	if (_override_modules != modules)
	{
		_override_modules = modules;
		_modified = true;
	}
}


/**
 * Load choices file if it exists
 */
void Choices::load()
{
	tbx::PropertySet ps;
	if (ps.load(choices_read_path("Choices")))
	{
		_update_prompt_days = ps.get("UpdatePromptDays", DEFAULT_PROMPT_DAYS);
		_enable_logging = ps.get("EnableLogging", false);
		_small_summary_bar = ps.get("SmallSummaryBar", false);
		std::string box = ps.get("MainWindowPosition", "");
		if (!box.empty())
		{
			std::istringstream is(box);
			char comma;
			tbx::BBox pos;
			is >> pos.min.x >> comma >> pos.min.y
			   >> comma >> pos.max.x >> comma >> pos.max.y;
			if (pos.min.x < pos.max.x - 4 && pos.min.y < pos.max.y - 4)
			{
				_main_window_pos = pos;
			}
		}
		_override_environment.clear();
		std::string envs( ps.get("OverrideEnvironment","") );
		if (!envs.empty())
		{
			std::string::size_type start = 0, pos;
			while ((pos = envs.find(',', start)) != std::string::npos)
			{
				_override_environment.insert(envs.substr(start, pos-start));
				start = pos + 1;
			}
			_override_environment.insert(envs.substr(start));
		}

		_override_modules.clear();
		std::string mods( ps.get("OverrideModules","") );
		if (!mods.empty())
		{
			std::string::size_type start = 0, pos;
			while ((pos = mods.find(',', start)) != std::string::npos)
			{
				_override_modules.insert(mods.substr(start, pos-start));
				start = pos + 1;
			}
			_override_modules.insert(mods.substr(start));
		}
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
	ps.set("EnableLogging", _enable_logging);
	ps.set("SmallSummaryBar", _small_summary_bar);
	std::ostringstream os;
	os << _main_window_pos.min.x << ',' << _main_window_pos.min.y
	   << ',' << _main_window_pos.max.x << ',' << _main_window_pos.max.y;
	ps.set("MainWindowPosition", os.str());

	std::ostringstream es;
	char comma = 0;
	for (const std::string &env : _override_environment)
	{
		if (comma) es << comma;
		else comma = ',';
		es << env;
	}
	ps.set("OverrideEnvironment", es.str());

	std::ostringstream ms;
	comma = 0;
	for (const std::string &mod : _override_modules)
	{
		if (comma) ms << comma;
		else comma = ',';
		ms << mod;
	}
	ps.set("OverrideModules", ms.str());

	if (ps.save(choices_write_path("Choices")))
	{
		_modified = false;
		return true;
	}

	return false;
}


/**
 * Ensure PackMan choices directory has been created
 */
bool Choices::ensure_choices_dir()
{
	tbx::Path choices_dir(ChoicesDir);
	try
	{
	   choices_dir.create_directory();
	   return true;
	} catch(...)
	{
		return false;
	}
}
