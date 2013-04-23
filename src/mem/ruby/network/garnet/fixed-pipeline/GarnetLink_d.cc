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

#include "mem/ruby/network/garnet/fixed-pipeline/CreditLink_d.hh"
#include "mem/ruby/network/garnet/fixed-pipeline/GarnetLink_d.hh"
#include "mem/ruby/network/garnet/fixed-pipeline/NetworkLink_d.hh"

GarnetIntLink_d::GarnetIntLink_d(const Params *p)
    : BasicLink(p)
{
    m_network_links[0] = p->network_links[0];
    m_credit_links[0] = p->credit_links[0];
    m_network_links[1] = p->network_links[1];
    m_credit_links[1] = p->credit_links[1];
}

void
GarnetIntLink_d::init()
{
}

void
GarnetIntLink_d::print(std::ostream& out) const
{
    out << name();
}

GarnetIntLink_d *
GarnetIntLink_dParams::create()
{
    return new GarnetIntLink_d(this);
}

GarnetExtLink_d::GarnetExtLink_d(const Params *p)
    : BasicLink(p)
{
    m_network_links[0] = p->network_links[0];
    m_credit_links[0] = p->credit_links[0];
    m_network_links[1] = p->network_links[1];
    m_credit_links[1] = p->credit_links[1];
}

void
GarnetExtLink_d::init()
{
}

void
GarnetExtLink_d::print(std::ostream& out) const
{
    out << name();
}

GarnetExtLink_d *
GarnetExtLink_dParams::create()
{
    return new GarnetExtLink_d(this);
}

#ifdef WARPED
warped::State*
GarnetIntLink_d::allocateState() {
  method_with_id(this->id);
  return new SimState();
}

void
GarnetIntLink_d::deallocateState( const warped::State* state ) {
  method_with_id(this->id);

  delete state;
}

void
GarnetIntLink_d::reclaimEvent( const warped::Event* event ){
  method_with_id(this->id);

  delete event;
}

void
GarnetIntLink_d::initialize() {
  method_with_id(this->id);
}

void
GarnetIntLink_d::executeProcess()  {
  method_with_id(this->id);
}

void
GarnetIntLink_d::finalize() {
  method_with_id(this->id);
}

warped::State*
GarnetExtLink_d::allocateState() {
  method_with_id(this->id);
  return new SimState();
}

void
GarnetExtLink_d::deallocateState( const warped::State* state ) {
  method_with_id(this->id);

  delete state;
}

void
GarnetExtLink_d::reclaimEvent( const warped::Event* event ){
  method_with_id(this->id);

  delete event;
}

void
GarnetExtLink_d::initialize() {
  method_with_id(this->id);
}

void
GarnetExtLink_d::executeProcess()  {
  method_with_id(this->id);
}

void
GarnetExtLink_d::finalize() {
  method_with_id(this->id);
}

#endif

