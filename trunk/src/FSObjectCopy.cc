
#include "FSObjectCopy.h"
#include "tbx/hourglass.h"

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
 * @param target full path for destination.
 *  The leaf name must match the sources leaf name and it must not already exist.
 */
FSObjectCopy::FSObjectCopy(const std::string &source, const std::string &target) :
   _source(source), _target_dir(target),
   _state(BUILD_LIST),
   _error(NO_ERROR),
   _warning(NO_WARNING),
   _byte_total(0),
   _file_total(0),
   _dir_total(0),
   _byte_done(0),
   _file_done(0),
   _dir_done(0)
{
	if (_source.leaf_name() != _target_dir.leaf_name())
	{
		_state = DONE;
		_error = LEAF_NAME_MISMATCH;
	} else if (_target_dir.exists())
	{
		_state = DONE;
		_error = TARGET_EXISTS;
	} else
	{
		_target_dir.up(); // Just need the destination directory for the target
	}
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
				_state = DONE;
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

	case UNWIND_COPY_OBJECT:
		// Delete any files copied already
		if (_copied.empty())
		{
			_state = DONE;
		} else
		{
			std::string object_to_delete = _copied.top();
			if (!delete_target_object(object_to_delete)) _warning = DELETE_FAILED;
			_to_copy.push(object_to_delete);
			_copied.pop();
		}
		break;

	case DONE:
		// Nothing to do - caller should stop polling now
		break;
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
	_source_dir = source.parent();
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

	if (info.directory())
	{
		if (target.create_directory())
		{
			_dir_done++;
			return true;
		}
	} else if (source.copy(target))
	{
		_byte_done += info.length();
		_file_done++;
		return true;
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
	return del_obj.remove();
}

/**
 * Return a number to give a guide to the total amount of work to do
 */
long long FSObjectCopy::total_cost()
{
	return (_dir_total + _file_total) *  FILEOP_COST + _byte_total;
}


/**
 * Return a number to give a guide to the total amount of work done
 */
long long FSObjectCopy::cost_done()
{
	return (_dir_done + _file_done) *  FILEOP_COST + _byte_done;
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
