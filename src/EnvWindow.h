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
