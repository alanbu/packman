/*********************************************************************
* This file is part of PackMan
*
* Copyright 2012-2020 AlanBuckley
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
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

