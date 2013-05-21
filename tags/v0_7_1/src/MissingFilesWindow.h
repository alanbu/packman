
#ifndef MISSINGFILESWINDOW_H_
#define MISSINGFILESWINDOW_H_

#include "tbx/window.h"
#include "StringListView.h"
#include <vector>

/**
 * Class to display the files from installed packages that no
 * longer exist on disk
 */
class MissingFilesWindow
{
	tbx::Window _window;
	StringListView _list;

public:
	MissingFilesWindow(const std::vector<std::string> &pathnames);
	virtual ~MissingFilesWindow();
};

#endif /* MISSINGFILESWINDOW_H_ */
