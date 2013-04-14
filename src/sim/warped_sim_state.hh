#ifndef __WARPED_SIM_STATE_HH__
#define __WARPED_SIM_STATE_HH__

#include <warped/State.h>

using namespace warped;

class SimState : public warped::State {
public:
  SimState();
  
  unsigned int getStateSize() const;
  void copyState( const warped::State* state );
};

#endif