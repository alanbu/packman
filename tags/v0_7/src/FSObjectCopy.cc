/*********************************************************************
* Copyright 2012 Alan Buckley
*
* This file is part of PackMan.
*
* PackMan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* PackMan is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with PackMan. If not, see <http://www.gnu.org/licenses/>.
*
*****************************************************************************/

#include "FSObjectCopy.h"
#include "tbx/hourglass.h"
#include "tbx/oserror.h"

//#include <iostream>
//#define FSO_LOG_ACTION(act, obj) std::cout << act << ":" << obj << std::endl;
#define FSO_LOG_ACTION(act, obj)

/** The cost of one file operation.
 * This is a conversion factor for equating the cost of a file operation
 * (such as rename or delete) with the cost of moving a given number of
 * bytes.  It is necessarily an approximate, but a good choice will
 * noticibly improve the linearity of the progress bar.
 */
const unsigned int FILEOP_COST=16384;

/**
 * Construct the FSObjectCopy.
 *
 * If there is a problem with the parameter, error() will be set and
 * the status set to DONE so it can be checked now or reported from
 * the poll.
 *
 * @param source full path of file or directory to copy
 * @param target full path of directory to copy it to.
 *  The leaf name must match the sources leaf name and it must not already exist.
 */
FSObjectCopy::FSObjectCopy(const std::string &source, const std::string &target) :
   _source(source), _target_dir(target),
   _delete_option(DO_NOT_DELETE),
   _state(BUILD_LIST),
   _error(NO_ERROR),
   _warning(NO_WARNING),
   _byte_total(0),
   _file_total(0),
   _dir_total(0),
   _byte_done(0),
   _file_done(0),
   _dir_done(0),
   _del_done(0),
   _unwind_done(0),
   _allow_dir_delete_failure(false),
   _image_dirs(0)
{
	_source_dir = _source.parent();
}

FSObjectCopy::~FSObjectCopy()
{
  delete _image_dirs;
}

/**
 * Add a child to the list of files to copy
 *
 * Calling this stops the automatic recursion of directories
 *
 * @param child_name name of child file relative to the source directory
 */
void FSObjectCopy::add_child_file(const std::string &child_name)
{
	std::string last_dir;
	if (_to_copy.empty())
	{
		// Make sure root directory is copied first
		last_dir = _source.leaf_name();
		_to_copy.push(last_dir);
		_dir_total++;
		if (_source.image_file())
		{
		   _image_dirs = new std::set<std::string>();
		   _image_dirs->insert(last_dir);
		}
	} else
	{
		std::string last_name = _to_copy.back();
		std::string::size_type last_dir_pos = last_name.rfind('.');
		last_dir = last_name.substr(0, last_dir_pos);
	}
	std::string rel_name = _source.leaf_name() + "." + child_name;

	std::string::size_type dir_pos = rel_name.rfind('.');
	if (rel_name.compare(0, dir_pos, last_dir) != 0)
	{
		// Add in parent directories
		do
		{
			dir_pos = rel_name.rfind('.', dir_pos-1);
			if (dir_pos < last_dir.size())
			{
				std::string::size_type ld_pos = last_dir.rfind('.');
				last_dir.erase(ld_pos);
			}
		} while (rel_name.compare(0, dir_pos, last_dir) != 0);

		while ((dir_pos = rel_name.find('.', dir_pos+1)) != std::string::npos)
		{
		    std::string dir_name = rel_name.substr(0, dir_pos);

			_to_copy.push(dir_name);
			_dir_total++;
			if (tbx::Path(_source_dir, dir_name).image_file())
			{
			   if (_image_dirs == 0) _image_dirs = new std::set<std::string>();
			   _image_dirs->insert(dir_name);
			}
		}
	}

	_to_copy.push(rel_name);
	_file_total++;

	tbx::PathInfo info;
	tbx::Path path(_source, child_name);
	if (path.path_info(info)) _byte_total += info.length();

	// Don't need to build list as files are being manually added
	if (_state == BUILD_LIST) _state = COPY_OBJECT;
	// We are adding files from within directories so we don't worry
	// about failing to delete the containing directory as this will
	// happen when they are not empty
	_allow_dir_delete_failure = true;
}


/**
 * Do actual copying a chunk at a time.
 *
 * Repeated call this routine until status() == DONE
 */
void FSObjectCopy::poll()
{
	switch(_state)
	{
	case BUILD_LIST:
		if (build_list()) _state = COPY_OBJECT;
		else
		{
			_error = BUILD_LIST_FAILED;
			_state = DONE;
		}
		break;

	case COPY_OBJECT:
		{
			if (_to_copy.empty())
			{
				if (_delete_option == DELETE_AFTER_COPY) _state = DELETE_SOURCE;
				else _state = DONE;
			} else
			{
				// Copy next file
				std::string object = _to_copy.front();
				if (copy_object(object))
				{
					_copied.push(object);
					_to_copy.pop();
				} else
				{
					_error = COPY_FAILED;
					_state = UNWIND_COPY_OBJECT;
				}
			}
		}
		break;

	case DELETE_SOURCE:
		if (_copied.empty())
		{
			_state = DONE;
		} else
		{
			std::string object = _copied.top();
			if (delete_source_object(object))
			{
			   _deleted.push(object);
			} else
			{
				_warning = DELETE_FAILED;
			}
			_del_done++;
			_copied.pop();
		}
		break;

	case UNWIND_COPY_OBJECT:
		// Delete any files copied already
		if (_copied.empty())
		{
			_state = DONE;
		} else
		{
			std::string object_to_delete = _copied.top();
			if (!delete_target_object(object_to_delete)) _warning = DELETE_TARGET_FAILED;
			_to_copy.push(object_to_delete);
			_copied.pop();
			_unwind_done++;
		}
		break;

	case UNWIND_MOVE_OBJECT:
		// Move any files already copied
		if (_deleted.empty())
		{
			// Set to delete the copied files
			_state = UNWIND_COPY_OBJECT;
		} else
		{
			std::string object_to_move = _deleted.top();
			if (!copy_object_back(object_to_move))
			{
				_warning = COPY_BACK_FAILED;
			} else
			{
				_copied.push(object_to_move);
			}
			_deleted.pop();
			_unwind_done++;
		}
		break;

	case DONE:
		// Nothing to do - caller should stop polling now
		break;
	}
}
/**
 * Set up to delete files if delete_option DELETE_ON_SECOND_PASS
 * is set. Just call poll() as normal to do the delete.
 */
void FSObjectCopy::start_delete_source()
{
	if (_delete_option == DELETE_ON_SECOND_PASS
			&& _state == DONE
			&& _error == NO_ERROR)
	{
		_state = DELETE_SOURCE;
	}
}

/**
 * Unwind any files copied so far.
 *
 * Used when a the copy is part of a failed transaction
 * and so the files copied need to be deleted.
 */
void FSObjectCopy::start_unwind_copy()
{
	if (!_copied.empty())
	{
		_state = UNWIND_COPY_OBJECT;
	} else
	{
		// Nothing to unwind
		_state = DONE;
	}
}

/**
 * Unwind files that are copied then deleted by
 * copying them back
 */
void FSObjectCopy::start_unwind_move()
{
	if (!_deleted.empty())
	{
		_state = UNWIND_MOVE_OBJECT;
		_warning = NO_WARNING;
	} else if (!_copied.empty())
	{
		_state = UNWIND_COPY_OBJECT;
		_warning = NO_WARNING;
	} else
	{
		// Nothing to unwind
		_state = DONE;
	}
}


/**
 * Build list of files and directories to copy
 *
 * @return true if list built successfully
 */
bool FSObjectCopy::build_list()
{
	tbx::Path source(_source);
	tbx::PathInfo info;
	if (source.path_info(info))
	{
		if (info.directory())
		{
			tbx::Hourglass hg;
			return add_directory(source.leaf_name());
		} else
		{
			_to_copy.push(source.leaf_name());
			_byte_total += info.length();
            _file_total++;
			return true;
		}
	} else
	{
		return false;
	}
}

/**
 * Recursive function to add a directory and its contents to
 * the list of files to copy.
 *
 * @param dirname directory name relative to the source path
 */
bool FSObjectCopy::add_directory(std::string dirname)
{
	_to_copy.push(dirname);
	_dir_total++;

	tbx::Path dir_path(_source_dir, dirname);
	std::vector<std::string> subdirs;
	for (tbx::PathInfo::Iterator i = tbx::PathInfo::begin(dir_path); i != tbx::PathInfo::end(); ++i)
	{
		tbx::PathInfo &info = *i;
		if (info.directory())
		{
			subdirs.push_back(dirname + "." + info.name());
		} else
		{
			_to_copy.push(dirname + "." + info.name());
			_byte_total += info.length();
			_file_total++;
		}
	}

	for (std::vector<std::string>::iterator d = subdirs.begin(); d != subdirs.end(); ++d)
	{
		if (!add_directory(*d)) return false;
	}
	return true;
}

/**
 * Copy object for source to destination
 *
 * @param name object name relative to source path
 */
bool FSObjectCopy::copy_object(const std::string &name)
{
	tbx::Path source(_source_dir, name);
	tbx::Path target(_target_dir, name);
	tbx::PathInfo info;

	if (!source.path_info(info)) return false;

	FSO_LOG_ACTION("copy_object", name);

    bool is_dir = info.directory();

    if (_image_dirs && info.image_file())
    {
       if (_image_dirs->find(name) != _image_dirs->end()) is_dir = true;
    }

	if (is_dir)
	{
	    try
	    {
		    target.create_directory();
			_dir_done++;
			return true;
		} catch(tbx::OsError &e)
		{
		   _error_msg = "Unable to create directory " + name + "\n" + e.what();
		}
	} else
	{
	    try
	    {
	       source.copy(target);
		   _byte_done += info.length();
		   _file_done++;
		   return true;
		} catch(tbx::OsError &e)
		{
		   _error_msg = "Unable to copy file " + name + "\n" + e.what();
	    }
	}

	return false;
}

/**
 * Copy object from destination to source
 *
 * Used to unwind a move
 *
 * @param name object name relative to source path
 */
bool FSObjectCopy::copy_object_back(const std::string &name)
{
	tbx::Path source(_source_dir, name);
	tbx::Path target(_target_dir, name);
	tbx::PathInfo info;
	if (!target.path_info(info)) return false;

	FSO_LOG_ACTION("copy_object_back", name);

	if (info.directory())
	{
	    try
	    {
		    source.create_directory();
			return true;
		} catch(...)
		{
		   // Ignore error
		}
	} else
	{
	    try
	    {
	       target.copy(source);
		   _unwind_done++;
		   return true;
		} catch(...)
		{
		   // Ignore error
		}
	}

	return false;
}


/**
 * Delete object from target folder
 *
 * @param name name relative to the target path
 * @return true if successful
 */
bool FSObjectCopy::delete_target_object(const std::string &name)
{
	tbx::Path del_obj(_target_dir, name);

	FSO_LOG_ACTION("delete_target_object", name);

    bool success = false;

    try
    {
	   del_obj.remove();
	   success = true;
	} catch(...)
	{
	   if (_allow_dir_delete_failure && del_obj.directory()) success = true;
	}
	return success;
}

/**
 * Delete object from source folder
 *
 * @param name name relative to the source path
 * @return true if successful
 */
bool FSObjectCopy::delete_source_object(const std::string &name)
{
	tbx::Path del_obj(_source_dir, name);

	FSO_LOG_ACTION("delete_source_object", name);

	bool success = false;
	try
	{
		// Extra checks to ensure installing inside and image file systems
		// doesn't delete the image file itself
		if (_image_dirs == 0 || !del_obj.image_file() || _image_dirs->find(name) == _image_dirs->end())
		{
			del_obj.remove();
		}
	   success = true;
	} catch(...)
	{
  	   if (_allow_dir_delete_failure && del_obj.directory()) success = true;
  	}
	return success;
}

/**
 * Return a number to give a guide to the total amount of work to do
 */
long long FSObjectCopy::total_cost()
{
	long long total_cost = (_dir_total + _file_total) *  FILEOP_COST + _byte_total;
	if (_delete_option != DO_NOT_DELETE) total_cost += (_dir_total + _file_total) *  FILEOP_COST;
	return total_cost;
}


/**
 * Return a number to give a guide to the total amount of work done
 */
long long FSObjectCopy::cost_done()
{
	return (_dir_done + _file_done + _del_done) *  FILEOP_COST + _byte_done;
}

/**
 * Return a number specifying the amount of work done
 * scaled to a range of 1-1000.
 */
int FSObjectCopy::scaled_done()
{
	long long total = total_cost();
	if (total == 0) return 0;
	return (int)((cost_done() * 1000) / total);
}

/**
 * Return the cost left to finish the unwind.
 *
 */
long long FSObjectCopy::unwind_cost()
{
	int num_objects = _dir_done + _file_done + _del_done;
	long long cost = 0;
	if (num_objects)
	{
		cost = ((num_objects - _unwind_done) * cost_done()) / num_objects;
	}
	return cost;
}
