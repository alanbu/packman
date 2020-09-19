/*********************************************************************
* This file is part of PackMan
*
* Copyright 2013-2020 AlanBuckley
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
 * BootOptionsWindow.cc
 *
 *  Created on: 9 Oct 2013
 *      Author: alanb
 */

#include "BootOptionsWindow.h"
#include "AppsWindow.h"
#include "tbx/application.h"
#include "tbx/actionbutton.h"
#include "tbx/path.h"
#include "tbx/messagewindow.h"
#include "libpkg/boot_options_file.h"

BootOptionsWindow::BootOptionsWindow(tbx::Object object) :
  _window(object),
  _look_at_option(_window.gadget(0)),
  _run_option(_window.gadget(1)),
  _add_to_apps_option(_window.gadget(2)),
  _apply_command(this, &BootOptionsWindow::apply)
{
	// 3 is cancel button - don't need to do anything with it
	tbx::ActionButton apply_button = _window.gadget(4);
	apply_button.add_select_command(&_apply_command);

	_window.add_about_to_be_shown_listener(this);
}

BootOptionsWindow::~BootOptionsWindow()
{
}

/**
 * Set up window for current application as it is shown
 */
void BootOptionsWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	AppsWindow *apps_window = AppsWindow::from_window(event.id_block().ancestor_object());
	_app_path = apps_window->selected_app_path();

	tbx::Path path(_app_path);
	std::string title = _window.title();
	std::string::size_type squote_pos = title.find('\'');
	std::string::size_type equote_pos = title.find('\'', squote_pos+1);
	if (squote_pos == std::string::npos) squote_pos = title.size();
	if (equote_pos == std::string::npos) equote_pos = title.size();

	title = title.substr(0, squote_pos+1) + path.leaf_name() + title.substr(equote_pos);
	_window.title(title.substr(0,127));

	pkg::look_at_options look_at;
	pkg::run_options run;
	pkg::add_to_apps_options add_to_apps;

	_look_at = look_at.contains(_app_path);
	_run = run.contains(_app_path);
	_add_to_apps = add_to_apps.contains(_app_path);

	_look_at_option.on(_look_at);
	_run_option.on(_run);
	_add_to_apps_option.on(_add_to_apps);
}

/**
 * Apply any changes made to the options
 */
void BootOptionsWindow::apply()
{
	bool boot = false;

	if (_look_at_option.on() != _look_at)
	{
		pkg::look_at_options look_at;
		if (_look_at)
		{
			look_at.remove(_app_path);
		} else
		{
			look_at.add(_app_path);
			boot = true;
		}
		look_at.commit();
	}

	if (_run_option.on() != _run)
	{
		pkg::run_options run;
		if (_run)
		{
			run.remove(_app_path);
		} else
		{
			run.add(_app_path);
			boot = true;
		}
		run.commit();
	}

	if (_add_to_apps_option.on() != _add_to_apps)
	{
		pkg::add_to_apps_options add_to_apps;
		if (_add_to_apps)
		{
			add_to_apps.remove(_app_path);
		} else
		{
			add_to_apps.add(_app_path);
			try
			{
				std::string command("AddApp ");
				command += _app_path;
				tbx::app()->os_cli(command);
			} catch (...)
			{
			   tbx::show_message("Unable to add the application/file to Apps");
			}

		}
		add_to_apps.commit();
	}

	if (boot)
	{
		std::string cmd("Filer_Boot ");
		cmd += _app_path;
		try
		{
		   tbx::app()->os_cli(cmd);
		} catch (...)
		{
		   tbx::show_message("Unable to Boot the application/file");
		}
	}
}
