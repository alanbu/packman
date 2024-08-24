/*********************************************************************
* This file is part of PackMan
*
* Copyright 2018-2020 AlanBuckley
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
 * EnvWindow.cc
 *
 *  Created on: 8 Jul 2018
 *      Author: alanb
 */

#include "EnvWindow.h"
#include "libpkg/env_checker.h"
#include "tbx/res/reswindow.h"
#include "tbx/res/resdisplayfield.h"
#include "tbx/res/resoptionbutton.h"
#include "tbx/optionbutton.h"
#include "tbx/displayfield.h"
#include "tbx/actionbutton.h"
#include "tbx/application.h"
#include "tbx/messagewindow.h"
#include "Choices.h"
#include "Packages.h"

EnvWindow *EnvWindow::_instance = nullptr;

enum EnvGadgets
{
	ID_OK = 1,
	ID_CANCEL = 2,
	ID_OVERRIDE = 3,
	ID_ENV_BOX = 0x10,
	ID_ENV_OPT = 0x11,
	ID_ENV_NAME = 0x12,
	ID_ENV_DESC = 0x13,
	ID_MOD_BOX = 0x1000,
	ID_MOD_OPT = 0x1001,
	ID_MOD_TITLE = 0x1002,
	ID_MOD_HELPSTRING = 0x1003
};


EnvWindow::EnvWindow() : _window("Environment"),
		_last_env_count(0),
		_last_mod_count(0),
		_override(false)
{
	// Load up measurements for resizing
	tbx::res::ResWindow res_env_wnd = tbx::app()->resource("Environment");

	_override_environment = choices().override_environment();
	_override_modules = choices().override_modules();

	_override = !_override_environment.empty();
	tbx::OptionButton override_button = _window.gadget(ID_OVERRIDE);
	override_button.on(_override);
	override_button.add_state_listener(this);

	RowAdder env_adder(_window, ID_ENV_OPT, _override, _override_environment);

	for(auto env_check_pair : pkg::env_checker::instance()->checks())
	{
		env_adder.add_row(env_check_pair.second);
		_last_env_count++;
	}

	tbx::BBox env_box = _window.gadget(ID_ENV_BOX).bounds();
	env_box.min.y += env_adder.bottom_row();
	_window.gadget(ID_ENV_BOX).bounds(env_box);

	for (int id = ID_MOD_BOX; id <= ID_MOD_HELPSTRING; ++id)
	{
		_window.gadget(id).move_by(0, env_adder.bottom_row());
	}

	int button_shift = env_adder.bottom_row();

	if (pkg::env_checker::instance()->module_checks().empty())
	{
		for (int id = ID_MOD_OPT; id <= ID_MOD_HELPSTRING; ++id)
		{
			_window.gadget(id).fade(true);
		}
	} else
	{
		RowAdder mod_adder(_window, ID_MOD_OPT, _override, _override_modules);
		for(auto mod_check_pair : pkg::env_checker::instance()->module_checks())
		{
			mod_adder.add_row(mod_check_pair.second);
			_last_mod_count++;
		}
		tbx::BBox mod_box = _window.gadget(ID_MOD_BOX).bounds();
		mod_box.min.y += mod_adder.bottom_row() -env_adder.bottom_row();
		_window.gadget(ID_MOD_BOX).bounds(mod_box);

		button_shift = mod_adder.bottom_row();
	}

	// Place the buttons
	tbx::ActionButton ok_button = _window.gadget(ID_OK);
	ok_button.move_by(0, button_shift);
	ok_button.add_selected_listener(this);
	tbx::ActionButton cancel_button = _window.gadget(ID_CANCEL);
	cancel_button.move_by(0, button_shift);

	tbx::BBox extent = _window.extent();
	extent.min.y += button_shift;
	_window.extent(extent);

	tbx::BBox wbox = _window.bounds();
	wbox.min.y += button_shift;
	wbox.move_to(wbox.min.x, 64);
	_window.bounds(wbox);
}

EnvWindow::~EnvWindow() {

}

void EnvWindow::show()
{
	if (_instance
		&& (_instance->_last_env_count < pkg::env_checker::instance()->checks().size()
		    || _instance->_last_mod_count < pkg::env_checker::instance()->module_checks().size()
			)
	   )
	{
		delete _instance;
		_instance = nullptr;
	}
	if (!_instance) _instance = new EnvWindow();
	_instance->_window.show();
}

/**
 * Override button selected
 */
void EnvWindow::option_button_state_changed(tbx::OptionButtonStateEvent &event)
{
	if (event.turned_on() != _override)
	{
		_override = event.turned_on();

		// Fade/Unfade button and restore detected state of environment if not overridden
		const pkg::env_checker::check_map &env_map = pkg::env_checker::instance()->checks();
		for (size_t i = 0; i < _last_env_count; ++i)
		{
			tbx::OptionButton opt_button(_window.gadget(ID_ENV_OPT + 3 * i));
			tbx::DisplayField name_field(_window.gadget(ID_ENV_NAME + 3 * i));
			opt_button.fade(!_override);
			if (!_override)
			{
				auto found = env_map.find(name_field.text());
				if (found != env_map.end())
				{
					opt_button.on(found->second->detected());
				} else
				{
					// Shouldn't get here
					opt_button.on(false);
				}

			}
		}

		if (_last_mod_count > 0)
		{
			// Fade/Unfade button and restore detected state of modules if not overridden
			const pkg::env_checker::check_map &mod_map = pkg::env_checker::instance()->module_checks();
			for (size_t i = 0; i < _last_mod_count; ++i)
			{
				tbx::OptionButton opt_button(_window.gadget(ID_MOD_OPT + 3 * i));
				tbx::DisplayField name_field(_window.gadget(ID_MOD_TITLE + 3 * i));
				opt_button.fade(!_override);
				if (!_override)
				{
					auto found = mod_map.find(name_field.text());
					if (found != mod_map.end())
					{
						opt_button.on(found->second->detected());
					} else
					{
						// Shouldn't get here
						opt_button.on(false);
					}
				}
			}
		}
	}
}

/**
 * OK button selected so check parameter and update environment
 */
void EnvWindow::button_selected(tbx::ButtonSelectedEvent &event)
{
	tbx::OptionButton override_button(_window.gadget(ID_OVERRIDE));

	if (override_button.on())
	{
		std::set<std::string> new_env;
		for(size_t i = 0; i < _last_env_count; ++i)
		{
			tbx::OptionButton opt_button(_window.gadget(ID_ENV_OPT + i * 3));
			if (opt_button.on())
			{
				tbx::DisplayField name_field(_window.gadget(ID_ENV_NAME + i * 3));
				new_env.insert(name_field.text());
			}
		}
		if (new_env.empty())
		{
			tbx::show_message_as_menu("You must select at least one environment","PackMan","error");
		} else
		{
			std::set<std::string> new_mods;
			for(size_t i = 0; i < _last_mod_count; ++i)
			{
				tbx::OptionButton opt_button(_window.gadget(ID_MOD_OPT + i * 3));
				if (opt_button.on())
				{
					tbx::DisplayField name_field(_window.gadget(ID_MOD_TITLE + i * 3));
					new_mods.insert(name_field.text());
				}
			}
			pkg::env_checker::instance()->override_environment(new_env, new_mods);
			choices().override_environment(new_env);
			choices().override_modules(new_mods);
			choices().save();
			_window.hide();
		}
	} else
	{
		if (!_override_environment.empty())
		{
			// Empty set is no override
			std::set<std::string> no_override;
			choices().override_environment(no_override);
			choices().override_modules(no_override);
			choices().save();
			pkg::env_checker::instance()->clear_environment_overrides();
		}
		_window.hide();
	}
}

/**
 * Add a check row to the window class
 */
EnvWindow::RowAdder::RowAdder(tbx::Window &window, int opt_id, bool override, std::set<std::string> &available) :
		_window(window),
		_opt_id(opt_id),
		_override(override),
		_available(available),
		_offset(0),
		_row_bottom(0),
		_res_window(tbx::app()->resource("Environment"))
{
}

/**
 * Add a new check
 */
void EnvWindow::RowAdder::add_row(pkg::env_check *check)
{
	bool available;
	if (_override)
	{
		available = (_available.count(check->name()) != 0);
	} else
	{
		available = check->available();
	}
	if (_offset)
	{
		tbx::res::ResOptionButton opt = _res_window.gadget(_opt_id);
		tbx::res::ResDisplayField name = _res_window.gadget(_opt_id + 1);
		tbx::res::ResDisplayField desc = _res_window.gadget(_opt_id + 2);
		_row_bottom -= 60;
		opt.component_id(_opt_id + _offset);
		opt.ymin(opt.ymin() + _row_bottom);
		opt.ymax(opt.ymax() + _row_bottom);
		opt.faded(!_override);
		opt.on(available);
		_window.add_gadget(opt);
		name.component_id(_opt_id + 1 + _offset);
		name.ymin(name.ymin() + _row_bottom);
		name.ymax(name.ymax() + _row_bottom);
		name.text(check->name());
		_window.add_gadget(name);
		desc.component_id(_opt_id + 2 + _offset);
		desc.ymin(desc.ymin() + _row_bottom);
		desc.ymax(desc.ymax() + _row_bottom);
		desc.text(check->description());
		_window.add_gadget(desc);
		_offset += 3;
	} else
	{
		tbx::OptionButton opt = _window.gadget(_opt_id);
		tbx::DisplayField name = _window.gadget(_opt_id + 1);
		tbx::DisplayField desc = _window.gadget(_opt_id + 2);
		opt.fade(!_override);
		opt.on(available);
		name.text(check->name());
		desc.text(check->description());
		_offset = 3;
		_row_bottom = opt.bottom_left().y - _res_window.gadget(_opt_id).ymin();
	}
}
