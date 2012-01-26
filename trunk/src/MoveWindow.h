/*
 * MoveWindow.h
 *
 *  Created on: 26 Jan 2012
 *      Author: alanb
 */

#ifndef MOVEWINDOW_H_
#define MOVEWINDOW_H_

#include "tbx/command.h"
#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/slider.h"
#include "MoveApp.h"

class MoveWindow : public tbx::Command
{
	tbx::Window _window;
	tbx::DisplayField _status_text;
	tbx::Slider _progress;

	MoveApp _move_app;

public:
	MoveWindow(const tbx::Path &app_path, const std::string &to_path);
	virtual void execute();

private:
	void close();
	void show_warning();
	void show_error();

	std::string warning_text();
	std::string error_text();
};

#endif /* MOVEWINDOW_H_ */
