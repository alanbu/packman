/*
 * CacheWindow.cc
 *
 *  Created on: 29 Sep 2011
 *      Author: alanb
 */

#include "CacheWindow.h"
#include "AbbrevSize.h"
#include "CommitWindow.h"
#include "tbx/application.h"
#include "tbx/path.h"
#include "tbx/stringutils.h"
#include "tbx/displayfield.h"
#include "tbx/ActionButton.h"
#include "libpkg/version.h"

#include <algorithm>
#include <cstdlib>

CacheWindow::CacheWindow(tbx::Object object) :
   _window(object),
   _info_view(_window, true),
   _clear_all(this, &CacheWindow::clear_all),
   _clear_old(this, &CacheWindow::clear_old)
{
	_window.add_about_to_be_shown_listener(this);
	_window.add_has_been_hidden_listener(this);
	tbx::ActionButton cabutton = _window.gadget(5);
	tbx::ActionButton cobutton = _window.gadget(6);
	cabutton.add_selected_command(&_clear_all);
	cobutton.add_selected_command(&_clear_old);
	_info_view.background(tbx::Colour(192,192,192));
	_info_view.margin(tbx::Margin(32));
	_info_view.text(tbx::app()->messages().gsmessage("CacheInfo",1024));
}

CacheWindow::~CacheWindow()
{
}


/**
 * Dialog about to be shown so recalculate cache statistics
 */
void CacheWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
   int total_items = 0;
   long long total_size = 0;
   tbx::PathInfo::Iterator pi;
   tbx::DisplayField items = _window.gadget(3);
   tbx::DisplayField old = _window.gadget(9);
   tbx::DisplayField size = _window.gadget(4);
   tbx::ActionButton cabutton = _window.gadget(5);
   tbx::ActionButton cobutton = _window.gadget(6);

   // Check packages variable has been defined
   const char *packages_dir = std::getenv("Packages$Dir");
   if (packages_dir != 0 && *packages_dir != 0)
   {
      for (pi = tbx::PathInfo::begin("<Packages$Dir>.Cache");
		   pi != tbx::PathInfo::end(); ++pi)
	   {
		   std::string leaf_name = pi->name();
		   std::string::size_type us_pos = leaf_name.rfind('_');
		   if (us_pos == std::string::npos || us_pos == 0 )
		   {
			   // Cant get version so just remember the file
			   _latest[leaf_name] = leaf_name;
		   } else
		   {
			   std::string name(leaf_name, 0, us_pos);
			   std::map<std::string, std::string>::iterator i = _latest.find(name);
			   if (i == _latest.end())
			   {
				   _latest[name] = leaf_name;
			   } else
			   {
				   std::string new_ver(leaf_name.substr(us_pos+1));
				   std::string old_ver(i->second.substr(us_pos+1));
				   std::replace(new_ver.begin(), new_ver.end(), '/', '.');
				   std::replace(old_ver.begin(), old_ver.end(), '/', '.');
				   pkg::version new_version(new_ver);
				   pkg::version old_version(old_ver);
				   if (new_version > old_version)
				   {
					   _old_versions.push_back(i->second);
					   _latest[name] = leaf_name;
				   } else
				   {
					   _old_versions.push_back(leaf_name);
				   }
			   }
		   }
		   total_items += 1;
		   total_size += pi->length();
	   }
   }


   items.text(tbx::to_string(total_items));
   old.text(tbx::to_string(_old_versions.size()));
   size.text(abbrev_size(total_size));

   bool fade_all = (total_items == 0);
   bool fade_old = _old_versions.empty();

   if (!fade_all)
   {
	   if (CommitWindow::showing())
	   {
		   CommitWindow *cw = CommitWindow::instance();
		   // Can still look at the cache if the commit window has finished
		   if (!cw->done())
		   {
			   fade_all = true;
			   fade_old = true;
		   }
	   }
   }

   cabutton.fade(fade_all);
   cobutton.fade(fade_old);
}

/**
 * Clear down list of files to clear when dialog is hidden
 */
void CacheWindow::has_been_hidden(const tbx::EventInfo &event)
{
	// Get rid of information remembered from the cache
	_old_versions.clear();
	_latest.clear();
}

/**
 * Clear down all items from the cache
 */
void CacheWindow::clear_all()
{
	tbx::Path cache_dir("<Packages$Dir>.Cache");
	for (std::map<std::string, std::string>::iterator i = _latest.begin();
			i != _latest.end(); ++i)
	{
		tbx::Path path(cache_dir, i->second);
		path.remove();
	}
}

/**
 * Clear the old versions from the cache
 */
void CacheWindow::clear_old()
{
	tbx::Path cache_dir("<Packages$Dir>.Cache");
	for (std::vector<std::string>::iterator i = _old_versions.begin();
			i != _old_versions.end(); ++i)
	{
		tbx::Path path(cache_dir, *i);
		path.remove();
	}
}
