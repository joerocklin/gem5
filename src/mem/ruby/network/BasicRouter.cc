/*
 * Copyright (c) 2011 Advanced Micro Devices, Inc.
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
 */

#include "mem/ruby/network/BasicRouter.hh"

BasicRouter::BasicRouter(const Params *p)
    : ClockedObject(p)
{
    m_id = p->router_id;
}

void
BasicRouter::init()
{
}

void
BasicRouter::print(std::ostream& out) const
{
    out << name();
}

BasicRouter *
BasicRouterParams::create()
{
    return new BasicRouter(this);
}

#ifdef WARPED
warped::State*
BasicRouter::allocateState() {
  method_with_id(this->id);
  return new SimState();
}

void
BasicRouter::deallocateState( const warped::State* state ) {
  method_with_id(this->id);

  delete state;
}

void
BasicRouter::reclaimEvent( const warped::Event* event ){
  method_with_id(this->id);

  delete event;
}

void
BasicRouter::initialize() {
  method_with_id(this->id);
}

void
BasicRouter::executeProcess()  {
  method_with_id(this->id);
}

void
BasicRouter::finalize() {
  method_with_id(this->id);
}

#endif
