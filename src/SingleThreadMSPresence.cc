// ----------------------------------------------------------------------
//
// SingleThreadMSPresence.cc
//
// Changes:
//
// 

#include "FWCore/MessageService/interface/SingleThreadMSPresence.h"
#include "FWCore/MessageService/interface/MessageLoggerScribe.h"

#include "FWCore/MessageLogger/interface/MessageLoggerQ.h"
#include "FWCore/MessageLogger/interface/MessageDrop.h"


namespace edm {
namespace service {


SingleThreadMSPresence::SingleThreadMSPresence()
  : Presence()
  , m(true)
{
  //std::cout << "SingleThreadMSPresence ctor\n";
  MessageLoggerQ::setMLscribe_ptr(&m);
  MessageDrop::instance()->messageLoggerScribeIsRunning = 
  				MLSCRIBE_RUNNING_INDICATOR;
}


SingleThreadMSPresence::~SingleThreadMSPresence()
{
  MessageLoggerQ::MLqEND();
  
}

} // end of namespace service  
} // end of namespace edm  