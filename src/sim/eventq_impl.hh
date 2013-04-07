/*
 * Copyright (c) 2012 The Regents of The University of Michigan
 * Copyright (c) 2012 Mark D. Hill and David A. Wood
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Steve Reinhardt
 *          Nathan Binkert
 *          Nilay Vaish
 */

#ifndef __SIM_EVENTQ_IMPL_HH__
#define __SIM_EVENTQ_IMPL_HH__

#include "base/trace.hh"
#include "sim/eventq.hh"

inline void
EventQueue::schedule(gem5::Event *event, Tick when)
{
    assert(when >= getCurTick());
    assert(!event->scheduled());
    assert(event->initialized());

    event->setWhen(when, this);
    insert(event);
    event->flags.set(gem5::Event::Scheduled);
    if (this == &mainEventQueue)
        event->flags.set(gem5::Event::IsMainQueue);
    else
        event->flags.clear(gem5::Event::IsMainQueue);

    if (DTRACE(Event))
        event->trace("scheduled");
}

inline void
EventQueue::deschedule(gem5::Event *event)
{
    assert(event->scheduled());
    assert(event->initialized());

    remove(event);

    event->flags.clear(gem5::Event::Squashed);
    event->flags.clear(gem5::Event::Scheduled);

    if (DTRACE(Event))
        event->trace("descheduled");

    if (event->flags.isSet(gem5::Event::AutoDelete))
        delete event;
}

inline void
EventQueue::reschedule(gem5::Event *event, Tick when, bool always)
{
    assert(when >= getCurTick());
    assert(always || event->scheduled());
    assert(event->initialized());

    if (event->scheduled())
        remove(event);

    event->setWhen(when, this);
    insert(event);
    event->flags.clear(gem5::Event::Squashed);
    event->flags.set(gem5::Event::Scheduled);
    if (this == &mainEventQueue)
        event->flags.set(gem5::Event::IsMainQueue);
    else
        event->flags.clear(gem5::Event::IsMainQueue);

    if (DTRACE(Event))
        event->trace("rescheduled");
}

#endif // __SIM_EVENTQ_IMPL_HH__
