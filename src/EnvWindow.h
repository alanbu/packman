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
 * EnvWindow.h
 *
 *  Created on: 8 Jul 2018
 *      Author: alanb
 */

#ifndef ENVWINDOW_H_
#define ENVWINDOW_H_

#include <tbx/window.h>
#include <tbx/optionbuttonstatelistener.h>
#include <tbx/buttonselectedlistener.h>
#include <tbx/res/reswindow.h>
#include <set>
#include <string>

namespace pkg
{
	class env_check;
}

class EnvWindow :
		tbx::OptionButtonStateListener,
		tbx::ButtonSelectedListener
{
	tbx::Window _window;
	static EnvWindow *_instance;
	size_t _last_env_count;
	size_t _last_mod_count;
	bool _override;
	std::set<std::string> _override_environment;
	std::set<std::string> _override_modules;
public:
	EnvWindow();
	virtual ~EnvWindow();

	static void show();

private:
	// Event handling
	virtual void option_button_state_changed(tbx::OptionButtonStateEvent &event);
	virtual void button_selected(tbx::ButtonSelectedEvent &event);

	/**
	 * Helper class for layout
	 */
	class RowAdder
	{
		tbx::Window &_window;
		int _opt_id;
		bool _override;
		std::set<std::string> &_available;
		int _offset;
		int _row_bottom;
		tbx::res::ResWindow _res_window;
	public:
		RowAdder(tbx::Window &window, int opt_id, bool override, std::set<std::string> &available);
		void add_row(pkg::env_check *check);
		int bottom_row() const {return _row_bottom;}
	};

};

#endif /* ENVWINDOW_H_ */
