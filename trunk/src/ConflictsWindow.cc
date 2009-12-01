/*
 * ConflictsWindow.cc
 *
 *  Created on: 23-Jul-2009
 *      Author: alanb
 */

#include "ConflictsWindow.h"
#include "tbx/deleteonhidden.h"

ConflictsWindow::ConflictsWindow(const std::set<std::string> &pathnames) :
	_window("Conflicts"),
	_list(_window)
{
	_list.assign(pathnames.begin(), pathnames.end());
	_list.autosize();

	// Delete window when it has been hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<ConflictsWindow>(this));
}

ConflictsWindow::~ConflictsWindow()
{
	_window.delete_object();
}
