/**
 * @file Sched.cc
 *
 * Imlpementation of event scheduler
 *
 * Author: Peter Helfer
 * Date: 2016-11-03
 */

#include <string>
#include "Sched.hh"

namespace Sched {
    enum EventDataType {
        NONE,
        UINT,
        DBLE,
        VOID_PTR
    };

    struct EventData {
        union {
            uint   u;
            double d;
            void   *v;
        };

        EventData() {}
        EventData(uint u)   : u(u) {}
        EventData(double d) : d(d) {}
        EventData(void *v)  : v(v) {}
    };

    union Callback {
        NoneCallback n;
        UintCallback u;
        DbleCallback d;
        VoidPtrCallback v;
    };

    /**
     * A scheduled event
     */
    struct Event {
        double        time;
        EventDataType type;
        Callback      cb;
        EventData     data;
        Event         *next;

        Event(
            double time,
            EventDataType type,
            Callback cb,
            EventData data)
            : time(time),
              type(type),
              cb(cb), 
              data(data), 
              next(NULL)
        {}
    };

    /**
     * List of scheduled events.
     */
    static Event *nextEvent = NULL;

    /**
     * Schedule an event
     * @param time Time for which event will be scheduled
     * @param cb Callback function
     * @param data Event Data
     */
    void scheduleEvent(
        double time, 
        EventDataType type,
        Callback cb,
        EventData data)
    {
        Event *newEv = new Event(time, type, cb, data);

        // Find event after which to insert new event
        //
        Event **p = &nextEvent;
        while ((*p != NULL) && ((*p)->time <= time)) {
            p = &((*p)->next);
        }
        newEv->next = *p;
        *p = newEv;
    }

    void scheduleEvent(
        double time, 
        NoneCallback ncb)
    {
        Callback cb;
        cb.n = ncb;
        scheduleEvent(time, NONE, cb, EventData());
    }

    void scheduleEvent(
        double time, 
        UintCallback ucb,
        uint data)
    {
        Callback cb;
        cb.u = ucb;
        EventData d;
        d.u = data;
        scheduleEvent(time, UINT, cb, d);
    }

    void scheduleEvent(
        double time, 
        DbleCallback dcb,
        double data)
    {
        Callback cb;
        cb.d = dcb;
        EventData d;
        d.d = data;
        scheduleEvent(time, DBLE, cb, d);
    }

    void scheduleEvent(
        double time, 
        VoidPtrCallback vcb,
        void *data)
    {
        Callback cb;
        cb.v = vcb;
        EventData d;
        d.v = data;
        scheduleEvent(time, VOID_PTR, cb, d);
    }
            
    /**
     * Clear all scheduled events
     */
    void clearEvents()
    {
        for (Event *ev = nextEvent; ev != NULL; ev = ev->next) {
            delete ev;
        }
        nextEvent = NULL;
    }

    /**
     * Process a scheduled event.
     */
    static void processEvent(Event *event, double now)
    {
        switch(event->type) {
            case NONE:
                event->cb.n(event->time, now);
                break;
            case UINT:
                event->cb.u(event->time, now, event->data.u);
                break;
            case DBLE:
                event->cb.d(event->time, now, event->data.d);
                break;
            case VOID_PTR:
                event->cb.v(event->time, now, event->data.v);
                break;
            default:
                abort();
        }
    }

    /**
     * Process all events scheduled at or before the specified time
     */
    void processEvents(double now)
    {
        while (nextEvent != NULL && nextEvent->time <= now) {
            Event *ev = nextEvent;
            processEvent(ev, now);
            nextEvent = ev->next;
            delete ev;
        }
    }
}
