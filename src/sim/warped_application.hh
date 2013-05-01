#ifndef __WARPED_APPLICATION_HH__
#define __WARPED_APPLICATION_HH__

#include <warped/Application.h>
#include <warped/IntVTime.h>
#include <warped/PartitionInfo.h>

#include "warped_utils.hh"

#include <vector>

class WarpedApplication : public warped::Application {
public:
  WarpedApplication();
  
  // The following are required as part of a Warped Application
  int finalize();
  int initialize( vector<string> &arguments );
  string getCommandLineParameters() const;
  int getNumberOfSimulationObjects(int mgrId) const;
  void registerDeserializers();
  const PartitionInfo *getPartitionInfo (unsigned int numProcessorsAvailable);
  
  const VTime &getPositiveInfinity(){ enter_method; return IntVTime::getIntVTimePositiveInfinity(); }
  const VTime &getZero(){ enter_method; return IntVTime::getIntVTimeZero(); }
  const VTime &getTime(string &){ enter_method; return IntVTime::getIntVTimeZero(); }

private:  
  // And now methods which are called from the above
  vector<SimulationObject *> *getSimulationObjects();
};

#endif
