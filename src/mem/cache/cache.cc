/*
 * Copyright (c) 2004-2005 The Regents of The University of Michigan
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
 * Authors: Erik Hallnor
 *          Steve Reinhardt
 *          Lisa Hsu
 *          Kevin Lim
 */

/**
 * @file
 * Cache template instantiations.
 */

#include "mem/config/cache.hh"

#if defined(USE_CACHE_LRU)
#include "mem/cache/tags/lru.hh"
#endif

#if defined(USE_CACHE_FALRU)
#include "mem/cache/tags/fa_lru.hh"
#endif

#include "mem/cache/cache_impl.hh"

// Template Instantiations
#ifndef DOXYGEN_SHOULD_SKIP_THIS


#if defined(USE_CACHE_FALRU)
template class Cache<FALRU>;
#endif

#if defined(USE_CACHE_LRU)
template class Cache<LRU>;
#endif

#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef WARPED
template <class TagStore> 
warped::State* 
Cache<TagStore>::allocateState() {
  method_with_id(this->id);
  return new SimState();
}

template <class TagStore>
void 
Cache<TagStore>::deallocateState( const warped::State* state ) {
  method_with_id(this->id);
  
  delete state;
}

template <class TagStore>
void
Cache<TagStore>::reclaimEvent( const warped::Event* event ){
  method_with_id(this->id);
  
  delete event;
}

template <class TagStore>
void
Cache<TagStore>::initialize() {
  method_with_id(this->id);
}

template <class TagStore>
void 
Cache<TagStore>::executeProcess()  {
  method_with_id(this->id);
}

template <class TagStore>
void 
Cache<TagStore>::finalize() {
  method_with_id(this->id);
}

#endif