/*
 * ComponentsMoved.h
 *
 *  Created on: 22 Oct 2013
 *      Author: alanb
 */

#ifndef COMPONENTSMOVED_H_
#define COMPONENTSMOVED_H_

#include <vector>
#include <set>
#include "tbx/path.h"

/**
 * Class to determine what components have been moved
 * by a change to the paths file and to update any
 * setting associated with the component
 */
class ComponentsMoved
{
public:
	enum State {UPDATE_BOOT_OPTIONS, BOOT_FILES, ADD_FILES_TO_APPS, DONE};
	enum Warning {NO_WARNING, BOOT_OPTIONS_UPDATE_FAILED, BOOTING_FAILED, ADDING_TO_APPS_FAILED};
private:
	/**
	 * Helper class to hold details of components to move
	 */
	class CompInfo
	{
	public:
		CompInfo() {}
		CompInfo(const std::string logical, const tbx::Path &current) : logical_path(logical), current_path(current) {};
		std::string logical_path;
		tbx::Path current_path;
		tbx::Path new_path;
	};
	std::vector<CompInfo> _components;

	State _state;
	Warning _warning;

	/** Files to be booted */
	std::set<std::string> _files_to_boot;
	/** Files to add to apps */
	std::set<std::string> _files_to_add_to_apps;

	int _poll_total;
	int _poll_count;

public:
	ComponentsMoved(tbx::Path path_to_move);
	virtual ~ComponentsMoved();

	void check_if_moved();

	void poll();

	/** Get the current state of the move */
	State state() const {return _state;}

	/** Get current warning */
	Warning warning() const {return _warning;}

	/** Number of times poll has been called */
	int poll_count() const {return _poll_count;}
	/** Maximum number of times poll will need to be called */
	int poll_total() const {return _poll_total;}
};

#endif /* COMPONENTSMOVED_H_ */
