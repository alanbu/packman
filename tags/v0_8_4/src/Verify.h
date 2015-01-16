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

#ifndef VERIFY
#define VERIFY

#include <string>
#include <vector>
#include <stack>

class Verify
{
public:
    enum State {BUILD_PACKAGE_LIST, NEXT_PACKAGE, BUILD_FILE_LIST, VERIFY_FILES, DONE};
    typedef std::pair<std::string, std::vector<std::string> > FailedPackage;

private:
    State _state;
    bool _cancel;
    std::stack<std::string> _packages;
    int _package_total;
    std::string _current;
    std::stack<std::string> _files;
    std::vector<std::string> _failed_files;
    int _file_total;
    std::vector<FailedPackage> _failed_packages;

public:
    Verify();

    State state() const {return _state;}
    const std::string &current() const {return _current;}
    int package_total() const {return _package_total;}
    int package_done() const {return _package_total + 1 - _packages.size();}
    int file_total() const {return _file_total;}
    int file_done() const {return _file_total - _files.size();}
    int failed_total() const {return _failed_packages.size();}
    const std::vector<FailedPackage> &failed_packages() const {return _failed_packages;}
    bool cancelled() const {return _cancel;}

    void poll();
    void cancel();

private:
    void build_package_list();
    void build_file_list();
};

#endif

