#ifndef Integration_Uselog4_h
#define Integration_Uselog4_h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/loglevel.h"


namespace edm {
  class ParameterSet;
}


namespace edmtest
{

class Uselog4
  : public edm::EDAnalyzer
{
public:
  explicit
    Uselog4( edm::ParameterSet const & );

  virtual
    ~Uselog4();

  virtual
    void analyze( edm::Event      const & e
                , edm::EventSetup const & c
                );

private:
  log4cplus::Logger loghere_;
};


}  // namespace edmtest


#endif  // Integration_ Uselog4_h
