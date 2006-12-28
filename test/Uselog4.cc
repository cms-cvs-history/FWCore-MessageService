#include "FWCore/MessageService/test/Uselog4.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "log4cplus/ndc.h"
#include "log4cplus/helpers/loglog.h"

#include <iostream>
#include <memory>

namespace edmtest
{

namespace {
  log4cplus::Logger getRootOne()
  {
     using namespace log4cplus;
     using namespace log4cplus::helpers;

     std::cerr << "getRootOne from Uselog4 called \n";
     SharedAppenderPtr ap(new ConsoleAppender());
     ap->setName("Main");
     ap->setLayout(std::auto_ptr<Layout>(new log4cplus::TTCCLayout()) );
     Logger::getRoot().addAppender(ap);
     // return Logger::getRoot();
     return Logger::getInstance("TestMe");
  }
}

  Uselog4::Uselog4( edm::ParameterSet const & ):
    loghere_(getRootOne())
  { 
    
  }

  Uselog4::~Uselog4()
  {
  }

void
  Uselog4::analyze( edm::Event      const & e
                              , edm::EventSetup const & /*unused*/
                              )
{
  //std::cout << "Module reached\n";
  log4cplus::getNDC().push("uselog4");

  LOG4CPLUS_INFO(loghere_,"A message containing exciting info.\n");
  LOG4CPLUS_WARN(loghere_,"A message containing an exciting warning.\n");
  LOG4CPLUS_ERROR(loghere_,"A message containing badness.\n");

  edm::LogWarning("aboutToSend") << "about to send 10 errors";
  for( unsigned i = 0;  i != 10;  ++i )  {
    edm::LogWarning("erfromu4") << "error number " << i;
  }

  log4cplus::getNDC().pop();

}  // Uselog4::analyze()


}  // namespace edmtest


using edmtest::Uselog4;
DEFINE_FWK_MODULE(Uselog4);
