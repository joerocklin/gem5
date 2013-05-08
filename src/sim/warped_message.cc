#include "warped_utils.hh"
#include "warped_message.hh"

#include <warped/IntVTime.h>
#include <warped/ObjectID.h>

using namespace warped;

// The constructor for DefaultEvent needs to be called as well
WarpedMessage::WarpedMessage(
                const VTime &sendTime,
                const VTime &recvTime,
                SimulationObject *sender,
                SimulationObject *receiver)
  : DefaultEvent(sendTime ,recvTime, sender, receiver) {
  enter_method;
}

// Constructor called by deserializer.
WarpedMessage::WarpedMessage(const VTime &sendTime,
                const VTime &recvTime,
                const ObjectID &sender,
                const ObjectID &receiver,
                unsigned int initEventId)
  :DefaultEvent(sendTime ,recvTime, sender, receiver, initEventId) {
  enter_method; 
}

const string &WarpedMessage::getDataType() const {
  enter_method;
  return this->getWarpedMessageDataType();
}

const string &WarpedMessage::getWarpedMessageDataType() {
  enter_method;
  static string WarpedMessageDataType = "WarpedMessage";
  return WarpedMessageDataType;
}

bool WarpedMessage::eventCompare(const Event* event) {
  enter_method;
  
  // Cast the parameter event to our type
  WarpedMessage *tutorialEvent = (WarpedMessage*) event;
  return (
    // Include this call to compare base-class parameters
    compareEvents(this, event) &&
    
    // Include any comparisons specific to WarpedMessage members
    this->sinkObject == tutorialEvent->sinkObject &&
    this->sourceObject == tutorialEvent->sourceObject
  );
}

void WarpedMessage::serialize(SerializedInstance *instance) const {
  enter_method;
  
  // Serialize the common event information
  Event::serialize( instance );
  
  // Add our information
  instance->addString(sinkObject);
  instance->addString(sourceObject);
}

Serializable *WarpedMessage::deserialize(SerializedInstance *instance) {
  enter_method;
  
  // Deserialize the basic event information.
  VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
  VTime *receiveTime = dynamic_cast<VTime *>(instance->getSerializable());
  unsigned int senderSimManID = instance->getUnsigned();
  unsigned int senderSimObjID = instance->getUnsigned();
  unsigned int receiverSimManID = instance->getUnsigned();
  unsigned int receiverSimObjID = instance->getUnsigned(); 
  unsigned int eventId = instance->getUnsigned();

  // Get the objects associated with the IDs received
  ObjectID sender(senderSimObjID, senderSimManID);
  ObjectID receiver(receiverSimObjID, receiverSimManID);

  // Create the message with this information
  WarpedMessage *message = new WarpedMessage(*sendTime, *receiveTime, 
        sender, receiver, eventId);
  
  // Deserialize our additions
  message->setSinkObject( instance->getString() );
  message->setSourceObject( instance->getString() );
  
  // Cleanup
  delete sendTime;
  delete receiveTime;
  
  return message;
}