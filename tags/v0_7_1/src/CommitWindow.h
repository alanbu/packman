/*
 * CommitWindow.h
 *
 *  Created on: 27-Mar-2009
 *      Author: alanb
 */

#ifndef COMMITWINDOW_H_
#define COMMITWINDOW_H_

#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/slider.h"
#include "tbx/actionbutton.h"
#include "tbx/command.h"

// libpkg commit.h doesn't currently define set and map and puts string
// in no namespace
#include <map>
#include <set>

using namespace std;
#include "libpkg/commit.h"
#include "libpkg/thread.h"


/**
 * Class to show a progress window and commit changes
 * from install
 */
class CommitWindow : public pkg::thread
{
	tbx::Window _window;
	tbx::DisplayField _action;
	tbx::Slider _progress;
	tbx::ActionButton _cancel_button;
    static CommitWindow *_instance;

    class CancelCommand : public tbx::Command
    {
    	CommitWindow *_me;
    public:
    	CancelCommand(CommitWindow *me) : _me(me) {};
    	virtual void execute();
    } _cancel_command;

    /**
     * Class to poll libpkg during null events
     */
    class PkgThreadRunner : public tbx::Command
    {
    public:
    	virtual void execute();
    } _thread_runner;

    pkg::commit *_commit;
    pkg::commit::state_type _state;

private:
    void open_conflicts_window();

public:
    CommitWindow();
	virtual ~CommitWindow();

    static bool showing() {return (_instance != 0);}
    static CommitWindow *instance() {return _instance;}
    bool done() const {return _commit == 0;}
    static bool running();

    void close();

    virtual void poll();
};

#endif /* COMMITWINDOW_H_ */
