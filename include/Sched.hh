/**
 * @file Sched.hh
 *
 * Author: Peter Helfer
 * Date: 2016-11-03
 */

#ifndef SCHED_HH
#define SCHED_HH

#include "Util.hh"

/**
 * Event scheduler
 */
namespace Sched {
    /**
     * Callback function signatures
     */
    typedef void (*NoneCallback)(double scheduledTime,
                                 double currentTime);

    typedef void (*UintCallback)(double scheduledTime,
                                 double currentTime,
                                 uint data);

    typedef void (*DbleCallback)(double scheduledTime,
                                 double currentTime,
                                 double data);

    typedef void (*VoidPtrCallback)(double scheduledTime,
                                 double currentTime,
                                 void *data);
    /**
     * Schedule an event
     * @param time Time for which event will be scheduled
     * @param cb Callback function
     * @param data Will be passed as parameter to cb
     */
    void scheduleEvent(
        double time, 
        NoneCallback cb);

    void scheduleEvent(
        double time, 
        UintCallback cb,
        uint data);

    void scheduleEvent(
        double time, 
        DbleCallback cb,
        double data);

    void scheduleEvent(
        double time, 
        VoidPtrCallback cb,
        void *data);

    /**
     * Clear all scheduled events
     */
    void clearEvents();

    /**
     * Process all events scheduled to run at or before the specified time
     */
    void processEvents(double time);
}

#endif
