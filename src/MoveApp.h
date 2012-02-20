
#ifndef MOVEAPP_H
#define MOVEAPP_H

#include "tbx/path.h"
#include "FSObjectCopy.h"

/**
 * Class to move an application on the disc and
 * update the package manager to the new location
 *
 * To use:
 *  Construct with from and to paths
 *  Repeatedly call poll() until status() is DONE or FAILED;
 *  If an error occurs FAILED is returned at the end and error()
 *  is set with the reason
 *  If it failed and couldn't restore things to the original state or
 *  could not delete the original app after the move a warning is
 *  return in warning()
 */
class MoveApp
{
public:
	enum State {START_BACKUP, BACKUP_FILES, START, COPYING_FILES, UPDATE_PATHS, UPDATE_VARS, DELETE_OLD_FILES, DONE, UNWIND_COPY, UNWIND_BACKUP, FAILED};
	enum Error {NO_ERROR, COPY_FAILED, PATH_UPDATE_FAILED, BACKUP_FAILED};
	enum Warning {NO_WARNING, DELETE_FAILED, UNWIND_PATHS_FAILED, UNWIND_COPY_FAILED};

private:
	std::string _logical_path;
	FSObjectCopy _copy_handler;
	FSObjectCopy *_backup_handler;
	State _state;
	Error _error;
	Warning _warning;
	long long _cost_done;
	long long _cost_total;
	long long _cost_stage_start;
	
public:
	MoveApp(const std::string &logical_path, const std::string &app_path, const std::string &to_path);
	~MoveApp();

	void poll();

	State state() const {return _state;}
	Error error() const {return _error;}
	Warning warning() const {return _warning;}
	int decipercent_done() const;

private:
	void setup_backup(const tbx::Path &target);
	void calc_cost_total(bool backup_only);
};

#endif
