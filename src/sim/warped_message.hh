#ifndef __WARPED_MESSAGE_HH__
#define __WARPED_MESSAGE_HH__

#include "warped_utils.hh"

#include <warped/DefaultEvent.h>
#include <warped/SerializedInstance.h>
#include <warped/IntVTime.h>

#include <iostream>
using namespace std;
using namespace warped;

class WarpedMessage : public DefaultEvent {
public:
  WarpedMessage(const VTime &sendTime,
                  const VTime &recvTime,
                  SimulationObject *sender,
                  SimulationObject *receiver);
                  
  const string &getDataType() const;
  bool eventCompare(const Event* event);
  unsigned int getEventSize() const { enter_method; return sizeof(WarpedMessage); };
  
  void serialize(SerializedInstance *instance) const;
  static Serializable *deserialize(SerializedInstance* instance);
  
  // Getters
  const string getSinkObject() const { return this->sinkObject; }
  const string getSourceObject() const { return this->sourceObject; }
  
  // Setters
  void setSinkObject(string id) { this->sinkObject = id; }
  void setSourceObject(string id) { this->sourceObject = id; }
  
private:
  string sourceObject;
  string sinkObject;
  
  // Constructor called by deserializer.
  WarpedMessage(const VTime &sendTime,
                  const VTime &recvTime,
                  const ObjectID &sender,
                  const ObjectID &receiver,
                  unsigned int initEventId);
  static const string &getWarpedMessageDataType();
  
};

#endif