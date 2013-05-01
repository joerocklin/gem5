#include "warped_application.hh"
#include "warped_utils.hh"

#include <warped/RoundRobinPartitioner.h>

#include <utils/ArgumentParser.h>

#include <vector>

using namespace std;
using namespace warped;

static unsigned int debug_level = 0;

WarpedApplication::WarpedApplication() {
  enter_method;
}

/** registerDeserializers
 * This is called after class creation.
 */
void WarpedApplication::registerDeserializers() {
  enter_method;
}

/** initialize
 * This is called after the deserializers are registered.
 * Common actions performed here include:
 *  - parsing configuration files
 */
int WarpedApplication::initialize( vector<string> &arguments ) {
  enter_method;
  
  return 0;
}
/** getNumberOfSimulationObjects
 * Called after initialize. 
 */
int WarpedApplication::getNumberOfSimulationObjects( int mgrId ) const {
  enter_method;

  return 0;
}


string WarpedApplication::getCommandLineParameters() const {
  enter_method;
  
  return "";
}

/** getPartitionInfo
 * After Warped has initialized the simulation, this method is called. This 
 * partitions the simulation objects based on the number of processors available.
 * 
 * NOTE - Other examples use this call to create the objects via a call to
 *        getSimulationObjects
 */
const PartitionInfo* WarpedApplication::getPartitionInfo( unsigned int numProcessorsAvailable ) {
  enter_method;
  
  if (debug_level > 2) { cout << "--- numProcessorsAvailable: " << numProcessorsAvailable << endl; }
  const PartitionInfo *ret_partition = 0;
  
  // Select a partitioning algorithm
  Partitioner *myPartitioner = new RoundRobinPartitioner();
  
  // Create some simulation objects and partition them  
  vector<SimulationObject *> *objects = getSimulationObjects();
  ret_partition = myPartitioner->partition( objects, numProcessorsAvailable );
  delete objects;  // 
  
  // WARNING: Returning NULL here will cause a segfault
  return ret_partition;
}

/** getSmulationObjects
 * Create the initial set of simulation objects
 */
vector<SimulationObject *> *WarpedApplication::getSimulationObjects() {
  enter_method;
  
  vector<SimulationObject *> *simObjects = new vector<SimulationObject *>;

  return simObjects;
}

int WarpedApplication::finalize() {
  enter_method;
  
  return 0;
}
