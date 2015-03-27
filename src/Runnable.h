/*
 * Runnable.h
 *
 *  Created on: Aug 21, 2009
 *      Author: keary
 */

#ifndef RUNNABLE_H_
#define RUNNABLE_H_

class Runnable {
public:
	Runnable(bool startSuspended = false) : _autoRemove(true), _suspended(startSuspended) { };
	virtual bool Run() = 0;
    virtual bool isRunning() { return true; }
    bool autoRemove() { return _autoRemove; }
    void Suspend() { _suspended = true; }
    void Resume() { _suspended = false; }
    bool isSuspended() { return _suspended; }
protected:
	bool _autoRemove;
	bool _suspended;
};

#endif /* SCHEDUABLE_H_ */
