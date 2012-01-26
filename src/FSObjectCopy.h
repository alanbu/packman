
#ifndef FSOBJECTCOPY_H
#define FSOBJECTCOPY_H

#include <string>
#include <queue>
#include <stack>

#include "tbx/path.h"

/**
 * Class to copy a file or directory to another location
 * using polling.
 *
 * The target should not exist before the copy
 *
 * To use:
 *   Create an instance with the source and target
 *   repeatedly call poll until status() == DONE
 *   Check error() for errors and warning() for warnings
 *   If an error occurs an attempt is made to delete any
 *   files copied so far - check warning() to see if it
 *   was successful.
 */
class FSObjectCopy
{
public:
	enum State {BUILD_LIST, COPY_OBJECT, UNWIND_COPY_OBJECT, DONE};
	enum Error {NO_ERROR, LEAF_NAME_MISMATCH, TARGET_EXISTS, BUILD_LIST_FAILED, COPY_FAILED };
	enum Warning {NO_WARNING, DELETE_FAILED};

private:
	tbx::Path _source;
	tbx::Path _target_dir;
	tbx::Path _source_dir;
    State _state;
	Error _error;
	Warning _warning;
	std::queue<std::string> _to_copy;
	std::stack<std::string> _copied;
    long long _byte_total;
    int _file_total;
    int _dir_total;
    long long _byte_done;
    int _file_done;
    int _dir_done;

public:
	FSObjectCopy(const std::string &source, const std::string &target);

	void poll();

	State state() {return _state;}
	Error error() {return _error;}
	Warning warning() {return _warning;}

	long long byte_total() {return _byte_total;}
	int file_total() {return _file_total;}
	int dir_total() {return _dir_total;}
	long long byte_done() {return _byte_done;}
	int file_done() {return _file_done;}
	int dir_done() {return _dir_done;}

	long long total_cost();
	long long cost_done();
	long long delete_source_cost();
	int scaled_done();

private:
	bool build_list();
	bool add_directory(std::string dirname);
	bool copy_object(const std::string &name);
	bool delete_target_object(const std::string &name);
};

#endif
