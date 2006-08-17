// ----------------------------------------------------------------------
//
// ELoutput.cc
//
//
// 7/8/98       mf      Created
// 6/10/99      jv      JV:1 puts a \n after each log using suppressContext()
// 6/11/99      jv      JV:2 accounts for newline at the beginning and end of
//                           an emitted ELstring
// 6/14/99      mf      Made the static char* in formatTime into auto so that
//                      ctime(&t) is called each time - corrects the bug of
//                      never changing the first timestamp.
// 6/15/99      mf      Inserted operator<<(void (*f)(ErrorLog&) to avoid
//                      mystery characters being inserted when users <<
//                      endmsg to an ErrorObj.
// 7/2/99       jv      Added separate/attachTime, Epilogue, and Serial options
// 8/2/99       jv      Modified handling of newline in an emmitted ELstring
// 2/22/00      mf      Changed usage of myDestX to myOutputX.  Added
//                      constructor of ELoutput from ELoutputX * to allow for
//                      inheritance.
// 6/7/00       web     Reflect consolidation of ELdestination/X; consolidate
//                      ELoutput/X; add filterModule() and query logic
// 10/4/00      mf      excludeModule()
// 1/15/01      mf      line length control: changed ELoutputLineLen to
//                      the base class lineLen (no longer static const)
// 2/13/01      mf      Added emitAtStart argument to two constructors
//                      { Enh 001 }.
// 4/4/01       mf      Simplify filter/exclude logic by useing base class
//                      method thisShouldBeIgnored().  Eliminate
//                      moduleOfinterest and moduleToexclude.
// 6/15/01      mf      Repaired Bug 005 by explicitly setting all
//                      ELdestination member data appropriately.
//10/18/01      mf      When epilogue not on separate line, preceed by space
// 6/23/03      mf      changeFile(), flush()
// 4/09/04      mf      Add 1 to length in strftime call in formatTime, to
//                      correctly provide the time zone.  Had been providing
//                      CST every time.
//
// 12/xx/06     mf	Tailoring to CMS MessageLogger 
//  1/11/06	mf      Eliminate time stamp from starting message 
//  3/20/06	mf	Major formatting change to do no formatting
//			except the header and line separation.
//  4/04/06	mf	Move the line feed between header and text
//			till after the first 3 items (FILE:LINE) for
//			debug messages. 
//  6/06/06	mf	Verbatim
//  6/12/06	mf	Set preambleMode true when printing the header
// ----------------------------------------------------------------------


#include "FWCore/MessageService/interface/ELoutput.h"
#include "FWCore/MessageService/interface/ELdestination.h"
#include "FWCore/MessageService/interface/ELadministrator.h"
#include "FWCore/MessageService/interface/ELcontextSupplier.h"

#include "FWCore/MessageLogger/interface/ErrorObj.h"
#include "FWCore/MessageLogger/interface/ELstring.h"


// Possible Traces:
// #define ELoutputCONSTRUCTOR_TRACE
// #define ELoutputTRACE_LOG
// #define ELoutput_EMIT_TRACE

#include <iostream>
#include <fstream>

namespace edm {
namespace service {

// ----------------------------------------------------------------------
// Useful function:
// ----------------------------------------------------------------------


static char * formatTime( const time_t t )  {

static char ts[] = "dd-Mon-yyyy hh:mm:ss XYZ";


#ifdef ANALTERNATIVE
  char * c  = ctime( &t );                      // 6/14/99 mf Can't be static!
  strncpy( ts+ 0, c+ 8, 2 );  // dd
  strncpy( ts+ 3, c+ 4, 3 );  // Mon
  strncpy( ts+ 7, c+20, 4 );  // yyyy
  strncpy( ts+12, c+11, 8 );  // hh:mm:ss
  strncpy( ts+21, tzname[localtime(&t)->tm_isdst], 3 );  // CST
#endif

  strftime( ts, strlen(ts)+1, "%d-%b-%Y %H:%M:%S %Z", localtime(&t) );
                // mf 4-9-04


  return ts;

}  // formatTime()


// ----------------------------------------------------------------------
// Constructors:
// ----------------------------------------------------------------------

ELoutput::ELoutput()
: ELdestination       (            )
, os                  ( &std::cerr )
, osIsOwned           ( false      )
, charsOnLine         ( 0          )
, xid                 (            )
, wantTimestamp       ( true       )
, wantModule          ( true       )
, wantSubroutine      ( true       )
, wantText            ( true       )
, wantSomeContext     ( true       )
, wantSerial          ( false      )
, wantFullContext     ( false      )
, wantTimeSeparate    ( false      )
, wantEpilogueSeparate( false      )
{

  #ifdef ELoutputCONSTRUCTOR_TRACE
    std::cerr << "Constructor for ELoutput()\n";
  #endif

  emit( "\n=================================================", true );
  emit( "\nMessage Log File written by MessageLogger service \n" );
  emit( "\n=================================================\n", true );

}  // ELoutput()


ELoutput::ELoutput( std::ostream & os_ , bool emitAtStart )
: ELdestination       (       )
, os                  ( &os_  )
, osIsOwned           ( false )
, charsOnLine         ( 0     )
, xid                 (       )
, wantTimestamp       ( true  )
, wantModule          ( true  )
, wantSubroutine      ( true  )
, wantText            ( true  )
, wantSomeContext     ( true  )
, wantSerial          ( false )
, wantFullContext     ( false )
, wantTimeSeparate    ( false )
, wantEpilogueSeparate( false )
{

  #ifdef ELoutputCONSTRUCTOR_TRACE
    std::cerr << "Constructor for ELoutput( os )\n";
  #endif

                                        // Enh 001 2/13/01 mf
  if (emitAtStart) {
    bool tprm = preambleMode;
    preambleMode = true;
    emit( "\n=================================================", true );
    emit( "\nMessage Log File written by MessageLogger service \n" );
    emit( "\n=================================================\n", true );
    preambleMode = tprm;
  }

}  // ELoutput()


ELoutput::ELoutput( const ELstring & fileName, bool emitAtStart )
: ELdestination       (       )
, os                  ( new std::ofstream( fileName.c_str()
                                         , std::ios/*_base*/::app
                                         )
                      )
, osIsOwned           ( false )
, charsOnLine         ( 0     )
, xid                 (       )
, wantTimestamp       ( true  )
, wantModule          ( true  )
, wantSubroutine      ( true  )
, wantText            ( true  )
, wantSomeContext     ( true  )
, wantSerial          ( false )
, wantFullContext     ( false )
, wantTimeSeparate    ( false )
, wantEpilogueSeparate( false )
{

  #ifdef ELoutputCONSTRUCTOR_TRACE
    std::cerr << "Constructor for ELoutput( " << fileName << " )\n";
  #endif

  bool tprm = preambleMode;
  preambleMode = true;
  if ( os && *os )  {
    #ifdef ELoutputCONSTRUCTOR_TRACE
      std::cerr << "          Testing if os is owned\n";
    #endif
    osIsOwned = true;
    #ifdef ELoutputCONSTRUCTOR_TRACE
      std::cerr << "          About to do first emit\n";
    #endif
                                        // Enh 001 2/13/01 mf
    if (emitAtStart) {
      emit( "\n=======================================================",
                                                                true );
      emit( "\nError Log File " );
      emit( fileName );
      emit( " \n" );
    }
  }
  else  {
    #ifdef ELoutputCONSTRUCTOR_TRACE
      std::cerr << "          Deleting os\n";
    #endif
    delete os;
    os = & std::cerr;
    #ifdef ELoutputCONSTRUCTOR_TRACE
      std::cerr << "          about to emit to cerr\n";
    #endif
    if (emitAtStart) {
      emit( "\n=======================================================",
                                                                true );
      emit( "\n%MSG** Logging to cerr is being substituted" );
      emit( " for specified log file \"" );
      emit( fileName  );
      emit( "\" which could not be opened for write or append.\n" );
    }
  }
  if (emitAtStart) {
    emit( formatTime(time(0)), true );
    emit( "\n=======================================================\n",
                                                                true );
  }
  preambleMode = tprm;

  #ifdef ELoutputCONSTRUCTOR_TRACE
    std::cerr << "Constructor for ELoutput completed.\n";
  #endif

}  // ELoutput()


ELoutput::ELoutput( const ELoutput & orig )
: ELdestination       (                           )
, os                  ( orig.os                   )
, osIsOwned           ( orig.osIsOwned            )
, charsOnLine         ( orig.charsOnLine          )
, xid                 ( orig.xid                  )
, wantTimestamp       ( orig.wantTimestamp        )
, wantModule          ( orig.wantModule           )
, wantSubroutine      ( orig.wantSubroutine       )
, wantText            ( orig.wantText             )
, wantSomeContext     ( orig.wantSomeContext      )
, wantSerial          ( orig.wantSerial           )
, wantFullContext     ( orig.wantFullContext      )
, wantTimeSeparate    ( orig.wantTimeSeparate     )
, wantEpilogueSeparate( orig.wantEpilogueSeparate )
{

  #ifdef ELoutputCONSTRUCTOR_TRACE
    std::cerr << "Copy constructor for ELoutput\n";
  #endif

  // mf 6/15/01 fix of Bug 005
  threshold             = orig.threshold;
  traceThreshold        = orig.traceThreshold;
  limits                = orig.limits;
  preamble              = orig.preamble;
  newline               = orig.newline;
  indent                = orig.indent;
  lineLength            = orig.lineLength;

  ignoreMostModules     = orig.ignoreMostModules;
  respondToThese        = orig.respondToThese;
  respondToMostModules  = orig.respondToMostModules;
  ignoreThese           = orig.ignoreThese;

  // ownership, if any, passes to new copy:
  const_cast<ELoutput &>(orig).osIsOwned = false;

}  // ELoutput()


ELoutput::~ELoutput()  {

  #ifdef ELoutputCONSTRUCTOR_TRACE
    std::cerr << "Destructor for ELoutput\n";
  #endif

  if ( osIsOwned )  { // we have an ofstream
    ((std::ofstream*)os)->close();
    delete os;
  }

}  // ~ELoutput()


// ----------------------------------------------------------------------
// Methods invoked by the ELadministrator:
// ----------------------------------------------------------------------

ELoutput *
ELoutput::clone() const  {

  return new ELoutput( *this );

} // clone()


bool ELoutput::log( const edm::ErrorObj & msg )  {

  #ifdef ELoutputTRACE_LOG
    std::cerr << "    =:=:=: Log to an ELoutput \n";
  #endif

  xid = msg.xid();      // Save the xid.

  // See if this message is to be acted upon, and add it to limits table
  // if it was not already present:
  //
  if ( xid.severity < threshold        )  return false;
  if ( thisShouldBeIgnored(xid.module) )  return false;
  if ( ! limits.add( xid )             )  return false;

  #ifdef ELoutputTRACE_LOG
    std::cerr << "    =:=:=: Limits table work done \n";
  #endif

  // Output the prologue:
  //
  preambleMode = true;

  if  ( !msg.is_verbatim() ) {
    emit( preamble );
    emit( xid.severity.getSymbol() );
    emit( " " );
    emit( xid.id );
    emit( msg.idOverflow() );
    emit( ": " );
  }
  
  #ifdef ELoutputTRACE_LOG
    std::cerr << "    =:=:=: Prologue done \n";
  #endif
  // Output serial number of message:
  //
  if  ( !msg.is_verbatim() ) {
    if ( wantSerial )  {
      std::ostringstream s;
      s << msg.serial();
      emit( "[serial #" + s.str() + ELstring("] ") );
    }
  }
  
#ifdef OUTPUT_FORMATTED_ERROR_MESSAGES
  // Output each item in the message (before the epilogue):
  //
  if ( wantText )  {
    ELlist_string::const_iterator it;
    for ( it = msg.items().begin();  it != msg.items().end();  ++it )  {
    #ifdef ELoutputTRACE_LOG
      std::cerr << "      =:=:=: Item:  " << *it << '\n';
    #endif
      emit( *it );
    }
  }
#endif

  // Provide further identification:
  //
  bool needAspace = true;
  if  ( !msg.is_verbatim() ) {
    if ( wantEpilogueSeparate )  {
      if ( xid.module.length() + xid.subroutine.length() > 0 )  {
	emit("\n");
	needAspace = false;
      }
      else if ( wantTimestamp && !wantTimeSeparate )  {
	emit("\n");
	needAspace = false;
      }
    }
    if ( wantModule && (xid.module.length() > 0) )  {
      if (needAspace) { emit(ELstring(" ")); needAspace = false; }
      emit( xid.module + ELstring(" ") );
    }
    if ( wantSubroutine && (xid.subroutine.length() > 0) )  {
      if (needAspace) { emit(ELstring(" ")); needAspace = false; }
      emit( xid.subroutine + "()" + ELstring(" ") );
    }
  }
  
  #ifdef ELoutputTRACE_LOG
    std::cerr << "    =:=:=: Module and Subroutine done \n";
  #endif

  // Provide time stamp:
  //
  if  ( !msg.is_verbatim() ) {
    if ( wantTimestamp )  {
      if ( wantTimeSeparate )  {
	emit( ELstring("\n") );
	needAspace = false;
      }
      if (needAspace) { emit(ELstring(" ")); needAspace = false; }
      emit( formatTime(msg.timestamp()) + ELstring(" ") );
    }
  }
  
  #ifdef ELoutputTRACE_LOG
    std::cerr << "    =:=:=: TimeStamp done \n";
  #endif

  // Provide the context information:
  //
  if  ( !msg.is_verbatim() ) {
    if ( wantSomeContext ) {
      if (needAspace) { emit(ELstring(" ")); needAspace = false; }
      #ifdef ELoutputTRACE_LOG
	std::cerr << "    =:=:=:>> context supplier is at 0x"
                  << std::hex
                  << &ELadministrator::instance()->getContextSupplier() << '\n';
	std::cerr << "    =:=:=:>> context is --- "
                  << ELadministrator::instance()->getContextSupplier().context()
                  << '\n';
      #endif
      if ( wantFullContext )  {
	emit( ELadministrator::instance()->getContextSupplier().fullContext());
      #ifdef ELoutputTRACE_LOG
	std::cerr << "    =:=:=: fullContext done: \n";
      #endif
      } else  {
	emit( ELadministrator::instance()->getContextSupplier().context());
    #ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: Context done: \n";
    #endif
      }
    }
  }
  
  // Provide traceback information:
  //

  bool insertNewlineAfterHeader = ( msg.xid().severity != ELsuccess );
  // ELsuccess is what LogDebug issues
  
  if  ( !msg.is_verbatim() ) {
    if ( msg.xid().severity >= traceThreshold )  {
      emit( ELstring("\n")
            + ELadministrator::instance()->getContextSupplier().traceRoutine()
          , insertNewlineAfterHeader );
    }
    else  {                                        //else statement added JV:1
      emit ("", insertNewlineAfterHeader);
    }
  }
  #ifdef ELoutputTRACE_LOG
    std::cerr << "    =:=:=: Trace routine done: \n";
  #endif

#ifndef OUTPUT_FORMATTED_ERROR_MESSAGES
  // Finally, output each item in the message:
  //
  preambleMode = false;
  if ( wantText )  {
    ELlist_string::const_iterator it;
    int item_count = 0;
    for ( it = msg.items().begin();  it != msg.items().end();  ++it )  {
    #ifdef ELoutputTRACE_LOG
      std::cerr << "      =:=:=: Item:  " << *it << '\n';
    #endif
      ++item_count;
      if  ( !msg.is_verbatim() ) {
	if ( !insertNewlineAfterHeader && (item_count == 3) ) {
          // in a LogDebug message, the first 3 items are FILE, :, and LINE
          emit( *it, true );
	} else {
          emit( *it );
	}
      } else {
        emit( *it );
      }
    }
  }
#endif

  // Done; message has been fully processed; separate, flush, and leave
  //

  (*os) << newline;
  flush(); 


  #ifdef ELoutputTRACE_LOG
    std::cerr << "  =:=:=: log(msg) done: \n";
  #endif

  return true;

}  // log()


// Remainder are from base class.

// ----------------------------------------------------------------------
// Output methods:
// ----------------------------------------------------------------------

void ELoutput::emit( const ELstring & s, bool nl )  {

  #ifdef ELoutput_EMIT_TRACE
    std::cerr << "[][][] in emit:  charsOnLine is " << charsOnLine << '\n';
    std::cerr << "[][][] in emit:  s.length() " << s.length() << '\n';
    std::cerr << "[][][] in emit:  lineLength is " << lineLength << '\n';
  #endif

  if (s.length() == 0)  {
    if ( nl )  {
      (*os) << newline << std::flush;
      charsOnLine = 0;
    }
    return;
  }

  char first = s[0];
  char second,
       last,
       last2;
  second = (s.length() < 2) ? '\0' : s[1];
  last = (s.length() < 2) ? '\0' : s[s.length()-1];
  last2 = (s.length() < 3) ? '\0' : s[s.length()-2];
         //checking -2 because the very last char is sometimes a ' ' inserted
         //by ErrorLog::operator<<

  if (preambleMode) {
               //Accounts for newline @ the beginning of the ELstring     JV:2
    if ( first == '\n'
    || (charsOnLine + static_cast<int>(s.length())) > lineLength )  {
      #ifdef ELoutput_EMIT_TRACE
	std::cerr << "[][][] in emit: about to << to *os \n";
      #endif
      (*os) << newline << indent;
      charsOnLine = indent.length();
      if (second != ' ')  {
	(*os) << ' ';
	charsOnLine++;
      }
      if ( first == '\n' )  {
	(*os) << s.substr(1);
      }
      else  {
	(*os) << s;
      }
    }
    #ifdef ELoutput_EMIT_TRACE
      std::cerr << "[][][] in emit: about to << s to *os: " << s << " \n";
    #endif
    else  {
      (*os) << s;
    }

    if (last == '\n' || last2 == '\n')  {  //accounts for newline @ end    $$ JV:2
      (*os) << indent;                    //of the ELstring
      if (last != ' ')
	(*os) << ' ';
      charsOnLine = indent.length() + 1;
    }

    if ( nl )  { (*os) << newline << std::flush; charsOnLine = 0;           }
    else       {                                 charsOnLine += s.length(); }
}

  if (!preambleMode) {
    (*os) << s;
  }
  
  #ifdef ELoutput_EMIT_TRACE
    std::cerr << "[][][] in emit: completed \n";
  #endif

}  // emit()


// ----------------------------------------------------------------------
// Methods controlling message formatting:
// ----------------------------------------------------------------------

void ELoutput::includeTime()   { wantTimestamp = true;  }
void ELoutput::suppressTime()  { wantTimestamp = false; }

void ELoutput::includeModule()   { wantModule = true;  }
void ELoutput::suppressModule()  { wantModule = false; }

void ELoutput::includeSubroutine()   { wantSubroutine = true;  }
void ELoutput::suppressSubroutine()  { wantSubroutine = false; }

void ELoutput::includeText()   { wantText = true;  }
void ELoutput::suppressText()  { wantText = false; }

void ELoutput::includeContext()   { wantSomeContext = true;  }
void ELoutput::suppressContext()  { wantSomeContext = false; }

void ELoutput::suppressSerial()  { wantSerial = false; }
void ELoutput::includeSerial()   { wantSerial = true;  }

void ELoutput::useFullContext()  { wantFullContext = true;  }
void ELoutput::useContext()      { wantFullContext = false; }

void ELoutput::separateTime()  { wantTimeSeparate = true;  }
void ELoutput::attachTime()    { wantTimeSeparate = false; }

void ELoutput::separateEpilogue()  { wantEpilogueSeparate = true;  }
void ELoutput::attachEpilogue()    { wantEpilogueSeparate = false; }


// ----------------------------------------------------------------------
// Summary output:
// ----------------------------------------------------------------------

void ELoutput::summarization(
  const ELstring & fullTitle
, const ELstring & sumLines
)  {
  const int titleMaxLength( 40 );

  // title:
  //
  ELstring title( fullTitle, 0, titleMaxLength );
  int q = (lineLength - title.length() - 2) / 2;
  ELstring line(q, '=');
  emit( "", true );
  emit( line );
  emit( " " );
  emit( title );
  emit( " " );
  emit( line, true );

  // body:
  //
  *os << sumLines;

  // finish:
  //
  emit( "", true );
  emit( ELstring(lineLength, '='), true );

}  // summarization()


// ----------------------------------------------------------------------
// Changing ostream:
// ----------------------------------------------------------------------

void ELoutput::changeFile (std::ostream & os_) {
    if ( osIsOwned )  { // we have an ofstream, close and delete it.
    ((std::ofstream*)os)->close();
    delete os;
  }
  os = &os_;
  osIsOwned = false;
  emit( "\n=======================================================", true );
  emit( "\nError Log changed to this stream\n" );
  emit( formatTime(time(0)), true );
  emit( "\n=======================================================\n", true );
}

void ELoutput::changeFile (const ELstring & filename) {
    if ( osIsOwned )  { // we have an ofstream, close and delete it.
    ((std::ofstream*)os)->close();
    delete os;
  }
  os = new std::ofstream( filename.c_str(), std::ios/*_base*/::app );
  osIsOwned = false;
  emit( "\n=======================================================", true );
  emit( "\nError Log changed to this file\n" );
  emit( formatTime(time(0)), true );
  emit( "\n=======================================================\n", true );
}

void ELoutput::flush()  {
  os->flush();
}


// ----------------------------------------------------------------------


} // end of namespace service  
} // end of namespace edm  
