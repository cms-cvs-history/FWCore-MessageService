// ----------------------------------------------------------------------
//
// HardwiredDefaults.cc
//
// All the hardwired defaults, organized by which JobMode they apply to
//
// JobModes:  
//	JobModeGrid
//	JobModeAnalysis
//
// Initial Version:  mf 6/18/07
//
// Changes:
//
// ----------------------------------------------------------------------



#include "FWCore/MessageService/interface/MessageLoggerDefaults.h"

#include <iostream>

namespace edm {
namespace service {

edm::JobMode 
MessageLoggerDefaults::
mode(std::string const & jm)
{
  if (jm == "")           return       GridJobMode;  // no -mode option = grid
  if (jm == "grid")       return       GridJobMode;
  if (jm == "analysis")   return   AnalysisJobMode;
  if (jm == "nil")        return        NilJobMode;
  std::cerr << "Unrecognized Job Mode Option: " << jm
  	    << " -- defaulting to analysis mode ";
  return AnalysisJobMode;
}  

void MessageLoggerDefaults::
hardwireGridJobMode() 
{
  destinations.push_back ( "cerr"              );
  categories.push_back   ( "FwkJob"            );
  categories.push_back   ( "FwkReport"         );
  categories.push_back   ( "FwkSummary"        );
  categories.push_back   ( "Root_NoDictionary" );
  statistics.push_back   ( "cerr"              );
  // cerr destination - 
  //      note that using the name cerr is OK since it is not std::cerr
  //      but when we want to use default as a name, we have to be circumspect
  { Destination cerr;			  // PSet cerr
    cerr.threshold = "INFO";		  // string threshold = "INFO"
    Category INFO_limits;		  // PSet INFO = { ... } 
      INFO_limits.limit = 0;		  // int32 limit = 0
      cerr.sev["INFO"] = INFO_limits; 
    Category  default_for_dest;		  // PSet default
      default_for_dest.limit = 10000000;  // int32 limit = 10000000
      cerr.category["default"] = default_for_dest;
    Category FwkReport;			  // PSet FwkReport
      FwkReport.limit = 10000000;	  // int32 limit = 10000000
      FwkReport.reportEvery = 1;	  // int32 reportEvery = 1
      cerr.category["FwkReport"] = FwkReport;
    Category FwkSummary;		  // PSet FwkSummary
      FwkSummary.limit = 10000000;	  // int32 limit = 10000000
      FwkSummary.reportEvery = 1;	  // int32 reportEvery = 1
      cerr.category["FwkSummary"] = FwkSummary;
    Category FwkJob;		  	  // PSet FwkJob
      FwkJob.limit = 0;	  		  // int32 limit = 0
      cerr.category["FwkJob"] = FwkJob;
     Category Root_NoDictionary;	  // PSet Root_NoDictionary
      Root_NoDictionary.limit =  0;	  // int32 limit = 0
      cerr.category["Root_NoDictionary"] = Root_NoDictionary;
    destination["cerr"] = cerr;
  }
  { Destination FrameworkJobReport;	  // PSet FrameworkJobReport
    Category  default_for_dest;		  // PSet default
      default_for_dest.limit = 0;  	  // int32 limit = 0
      FrameworkJobReport.category["default"] = default_for_dest;
    Category FwkJob;		  	  // PSet FwkJob
      FwkJob.limit = 10000000;	  	  // int32 limit = 10000000
      FrameworkJobReport.category["FwkJob"] = FwkJob;
    destination["FrameworkJobReport"] = FrameworkJobReport;
  }
} // hardwireGridJobMode

void MessageLoggerDefaults::
hardwireAnalysisJobMode() 
{
  destinations.push_back ( "warnings"          );
  categories.push_back   ( "FwkJob"            );
  categories.push_back   ( "FwkReport"         );
  categories.push_back   ( "FwkSummary"        );
  categories.push_back   ( "Root_NoDictionary" );
  statistics.push_back   ( "cerr"              );
  // cerr destination - 
  //      note that using the name cerr is OK since it is not std::cerr
  //      but when we want to use default as a name, we have to be circumspect
  { Destination warnings;		  // PSet warnings
    warnings.threshold = "INFO";	  // string threshold = "INFO"
    Category INFO_limits;		  // PSet INFO = { ... } 
      INFO_limits.limit = 0;		  // int32 limit = 0
      warnings.sev["INFO"] = INFO_limits; 
    Category  default_for_dest;		  // PSet default
      default_for_dest.limit = 1000    ;  // int32 limit = 1000
      warnings.category["default"] = default_for_dest;
    Category FwkReport;			  // PSet FwkReport
      FwkReport.limit = 10000000;	  // int32 limit = 10000000
      FwkReport.reportEvery = 100;	  // int32 reportEvery = 100
      warnings.category["FwkReport"] = FwkReport;
    Category FwkSummary;		  // PSet FwkSummary
      FwkSummary.limit = 10000000;	  // int32 limit = 10000000
      warnings.category["FwkSummary"] = FwkSummary;
    Category FwkJob;		  	  // PSet FwkJob
      FwkJob.limit = 0;	  		  // int32 limit = 0
      warnings.category["FwkJob"] = FwkJob;
     Category Root_NoDictionary;	  // PSet Root_NoDictionary
      Root_NoDictionary.limit =  0;	  // int32 limit = 0
      warnings.category["Root_NoDictionary"] = Root_NoDictionary;
    destination["warnings"] = warnings;
  }
} // hardwireAnalysisJobMode

void MessageLoggerDefaults::
hardwireNilJobMode() 
{
} // hardwireNilJobMode


} // end of namespace service  
} // end of namespace edm  
