/*
 * CacheWindow.h
 *
 *  Created on: 29 Sep 2011
 *      Author: alanb
 */

#ifndef CACHEWINDOW_H_
#define CACHEWINDOW_H_

#include "tbx/window.h"
#include "tbx/view/textview.h"
#include "tbx/abouttobeshownlistener.h"
#include "tbx/hasbeenhiddenlistener.h"
#include "tbx/Command.h"
#include <vector>
#include <map>

class CacheWindow : tbx::AboutToBeShownListener, tbx::HasBeenHiddenListener
{
	tbx::Window _window;
	tbx::view::TextView _info_view;
	std::vector<std::string> _old_versions;
	std::map<std::string,std::string> _latest;
	tbx::CommandMethod<CacheWindow> _clear_all;
	tbx::CommandMethod<CacheWindow> _clear_old;
public:
	CacheWindow(tbx::Object object);
	virtual ~CacheWindow();

	void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	void has_been_hidden(const tbx::EventInfo &event);

	void clear_all();
	void clear_old();
};

#endif /* CACHEWINDOW_H_ */
