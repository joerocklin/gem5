#include "sim_state.h"

#include <warped/warped.h> // for ASSERT

SimState::SimState() {
  
}

unsigned int SimState::getStateSize() const {
  return sizeof(SimState);
}

void SimState::copyState(const State* state) {
  ASSERT( state != 0 );
  const SimState *myState = dynamic_cast<const SimState *>(state);
}