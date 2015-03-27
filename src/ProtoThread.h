/*
 * ProtoThread.h
 *
 *  Created on: Aug 21, 2009
 *      Author: keary
 */

#ifndef PROTOTHREAD_H_
#define PROTOTHREAD_H_

#include <stdlib.h>
#include "Runnable.h"
#include "lc-addrlabels.h"

class ProtoThread: public Runnable {
public:
	ProtoThread(bool startSuspended = false) : Runnable(startSuspended) { Restart(); }
    // Construct a new protothread that will start from the beginning
    // of its Run() function.

	// Restart protothread.
    void Restart() { LC_INIT(lc); }

    // Stop the protothread from running. Happens automatically at PT_END.
    // Note: this differs from the Dunkels' original protothread behaviour
    // (his restart automatically, which is usually not what you want).
    void Stop() { Restart(); }

    // Return true if the protothread is running or waiting, false if it has
    // ended or exited.
    bool IsRunning() { return lc != 0; }

    // Run next part of protothread or return immediately if it's still
    // waiting. Return true if protothread is still running, false if it
    // has finished. Implement this method in your Protothread subclass.
    virtual bool Run() = 0;
protected:
	lc_t lc;
};

class PTSemaphore {
	friend class ProtoThread;
public:
	PTSemaphore(int cnt = 1) : count(cnt) { };
	int Signal() { return ++count; }
protected:
	unsigned int count;
};

class PTMutex : public PTSemaphore {
public:
	PTMutex(bool locked=false) : PTSemaphore(locked ? 0 : 1) { };
	void Release() { if (isLocked()) Signal(); }
	bool isLocked() { return count == 0; }
};
/**
 * \name Initialization
 * @{
 */


/**
 * Declare the start of a protothread inside the C function
 * implementing the protothread.
 *
 * This macro is used to declare the starting point of a
 * protothread. It should be placed at the start of the function in
 * which the protothread runs. All C statements above the PT_BEGIN()
 * invokation will be executed each time the protothread is scheduled.
 *
 * \param pt A pointer to the protothread control structure.
 *
 * \hideinitializer
 */
#define PT_BEGIN() { char PT_YIELD_FLAG = 1; LC_RESUME(lc)

/**
 * Declare the end of a protothread.
 *
 * This macro is used for declaring that a protothread ends. It must
 * always be used together with a matching PT_BEGIN() macro.
 *
 * \param pt A pointer to the protothread control structure.
 *
 * \hideinitializer
 */
#define PT_END() LC_END(lc); PT_YIELD_FLAG = 0; \
                   Restart(); return false; }

/** @} */

/**
 * \name Blocked wait
 * @{
 */

/**
 * Block and wait until condition is true.
 *
 * This macro blocks the protothread until the specified condition is
 * true.
 *
 * \param pt A pointer to the protothread control structure.
 * \param condition The condition.
 *
 * \hideinitializer
 */
#define PT_WAIT_UNTIL(condition)	        \
  do {						\
    LC_SET(lc);				\
    if(!(condition)) {				\
      return true;			\
    }						\
  } while(0)

/**
 * Block and wait while condition is true.
 *
 * This function blocks and waits while condition is true. See
 * PT_WAIT_UNTIL().
 *
 * \param pt A pointer to the protothread control structure.
 * \param cond The condition.
 *
 * \hideinitializer
 */
#define PT_WAIT_WHILE(cond)  PT_WAIT_UNTIL(!(cond))

/** @} */

/**
 * \name Hierarchical protothreads
 * @{
 */

/**
 * Block and wait until a child protothread completes.
 *
 * This macro schedules a child protothread. The current protothread
 * will block until the child protothread completes.
 *
 * \note The child protothread must be manually initialized with the
 * PT_INIT() function before this function is used.
 *
 * \param pt A pointer to the protothread control structure.
 * \param thread The child protothread with arguments
 *
 * \sa PT_SPAWN()
 *
 * \hideinitializer
 */
#define PT_WAIT_THREAD(child) PT_WAIT_WHILE(PT_SCHEDULE(child))

/**
 * Spawn a child protothread and wait until it exits.
 *
 * This macro spawns a child protothread and waits until it exits. The
 * macro can only be used within a protothread.
 *
 * \param pt A pointer to the protothread control structure.
 * \param child A pointer to the child protothread's control structure.
 * \param thread The child protothread with arguments
 *
 * \hideinitializer
 */
#define PT_SPAWN(child)		\
  do {						\
    (child)->Restart();				\
    PT_WAIT_THREAD((child));		\
  } while(0)

/** @} */

/**
 * \name Exiting and restarting
 * @{
 */

/**
 * Restart the protothread.
 *
 * This macro will block and cause the running protothread to restart
 * its execution at the place of the PT_BEGIN() call.
 *
 * \param pt A pointer to the protothread control structure.
 *
 * \hideinitializer
 */
#define PT_RESTART()				\
  do {						\
    Restart();				\
    return true;			\
  } while(0)

/**
 * Exit the protothread.
 *
 * This macro causes the protothread to exit. If the protothread was
 * spawned by another protothread, the parent protothread will become
 * unblocked and can continue to run.
 *
 * \param pt A pointer to the protothread control structure.
 *
 * \hideinitializer
 */
#define PT_EXIT()				\
  do {						\
    Restart();				\
    return false;			\
  } while(0)

/** @} */

/**
 * \name Calling a protothread
 * @{
 */

/**
 * Schedule a protothread.
 *
 * This function schedules a protothread. The return value of the
 * function is non-zero if the protothread is running or zero if the
 * protothread has exited.
 *
 * \param f The call to the C function implementing the protothread to
 * be scheduled
 *
 * \hideinitializer
 */
#define PT_SCHEDULE(child) ((child)->Run())

/** @} */

/**
 * \name Yielding from a protothread
 * @{
 */

/**
 * Yield from the current protothread.
 *
 * This function will yield the protothread, thereby allowing other
 * processing to take place in the system.
 *
 * \param pt A pointer to the protothread control structure.
 *
 * \hideinitializer
 */
#define PT_YIELD()				\
  do {						\
    PT_YIELD_FLAG = 0;				\
    LC_SET(lc);				\
    if(PT_YIELD_FLAG == 0) {			\
      return true;			\
    }						\
  } while(0)

/**
 * \brief      Yield from the protothread until a condition occurs.
 * \param pt   A pointer to the protothread control structure.
 * \param cond The condition.
 *
 *             This function will yield the protothread, until the
 *             specified condition evaluates to true.
 *
 *
 * \hideinitializer
 */
#define PT_YIELD_UNTIL(cond)		\
  do {						\
    PT_YIELD_FLAG = 0;				\
    LC_SET(lc);				\
    if((PT_YIELD_FLAG == 0) || !(cond)) {	\
      return true;			\
    }						\
  } while(0)

/**
 * Initialize a semaphore
 *
 * This macro initializes a semaphore with a value for the
 * counter. Internally, the semaphores use an "unsigned int" to
 * represent the counter, and therefore the "count" argument should be
 * within range of an unsigned int.
 *
 * \param s (struct pt_sem *) A pointer to the pt_sem struct
 * representing the semaphore
 *
 * \param c (unsigned int) The initial count of the semaphore.
 * \hideinitializer
 */
//#define PT_SEM_INIT(s, c) (s)->count = c

/**
 * Wait for a semaphore
 *
 * This macro carries out the "wait" operation on the semaphore. The
 * wait operation causes the protothread to block while the counter is
 * zero. When the counter reaches a value larger than zero, the
 * protothread will continue.
 *
 * \param pt (struct pt *) A pointer to the protothread (struct pt) in
 * which the operation is executed.
 *
 * \param s (struct pt_sem *) A pointer to the pt_sem struct
 * representing the semaphore
 *
 * \hideinitializer
 */
#define PT_SEM_WAIT(s)	\
  do {						\
    PT_WAIT_UNTIL((s)->count > 0);		\
    --(s)->count;				\
  } while(0)

/**
 * Signal a semaphore
 *
 * This macro carries out the "signal" operation on the semaphore. The
 * signal operation increments the counter inside the semaphore, which
 * eventually will cause waiting protothreads to continue executing.
 *
 * \param pt (struct pt *) A pointer to the protothread (struct pt) in
 * which the operation is executed.
 *
 * \param s (struct pt_sem *) A pointer to the pt_sem struct
 * representing the semaphore
 *
 * \hideinitializer
 */
#define PT_SEM_SIGNAL(s) ++(s)->count

#define PT_MUTEX_ACQUIRE(s) PT_SEM_WAIT(s)
#define PT_MUTEX_RELEASE(s) (s->Release())

#endif /* PROTOTHREAD_H_ */
