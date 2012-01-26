
#include "tbx/application.h"
#include "tbx/reporterror.h"
#include "MoveWindow.h"


MoveWindow::MoveWindow(const tbx::Path &app_path, const std::string &to_path) :
  _window("MoveWindow"),
  _move_app(app_path, to_path)
{
	_status_text = _window.gadget(1);
	_progress = _window.gadget(2);

	_window.show();
	
	tbx::app()->add_idle_command(this);
}

void MoveWindow::execute()
{
	_move_app.poll();

	_progress.value(_move_app.decipercent_done());

	switch (_move_app.state())
	{
	case MoveApp::START: break; // Here to get rid of compiler warning only

	case MoveApp::COPYING_FILES:
		_status_text.text("Copying files");
		break;

	case MoveApp::UPDATE_PATHS:
		_status_text.text("Updating paths file");
		break;

	case MoveApp::UPDATE_VARS:
		_status_text.text("Updating system variables");
		break;

	case MoveApp::DELETE_OLD_FILES:
		_status_text.text("Deleting original files");
		break;

	case MoveApp::DONE:
		if (_move_app.warning()) show_warning();
		close();
		break;

	case MoveApp::UNWIND_PATHS:
		_status_text.text("Unwinding after error - restoring paths");
		break;

	case MoveApp::UNWIND_COPY:
		_status_text.text("Unwinding after error - deleting copied files");
		break;

	case MoveApp::FAILED:
		show_error();
		close();
		break;
	}
}

/**
 * Close down the window when move has finished
 */
void MoveWindow::close()
{
	tbx::app()->remove_idle_command(this);
	_window.hide();
	_window.delete_object();
	delete this;
}

void MoveWindow::show_warning()
{
	std::string msg("Warning: ");
	msg += warning_text();
	//TODO: show_message(msg);
	tbx::report_error(msg);
}

void MoveWindow::show_error()
{
	std::string msg("Error move failed: ");
	msg += error_text();
	//TODO: show_message(msg);
	tbx::report_error(msg);
}

std::string MoveWindow::warning_text()
{
	std::string text("TODO: warning text");
	return text;
}

std::string MoveWindow::error_text()
{
	std::string text("TODO: error text");
	return text;
}
