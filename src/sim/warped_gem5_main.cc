// See copyright notice in file Copyright in the root directory of this archive.

// sim/init.hh MUST be included first because it includes Python.h
// which defines several macros which must be defined before including
// other system libraries.
#include "sim/init.hh"

#include "warped_gem5_main.hh"

#include <warped/Spinner.h>
#include <utils/ArgumentParser.h>
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <cstdio>
#include <algorithm>

// We need these to register their deserializers
#include <warped/CirculateInitializationMessage.h>
#include <warped/DeserializerManager.h>
#include <warped/EventMessage.h>
#include <warped/GVTUpdateMessage.h>
#include <warped/InitializationMessage.h>
#include <warped/UsefulWorkMessage.h>
#include <warped/IntVTime.h>
#include <warped/MatternGVTMessage.h>
#include <warped/NegativeEventMessage.h>
#include <warped/NegativeEvent.h>
#include <warped/RestoreCkptMessage.h>
#include <warped/StartMessage.h>
#include <warped/TerminateToken.h>

using std::string;
using std::fstream;
using std::ofstream;
using std::ifstream;
using std::istringstream;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using namespace warped;

// maximum length of command line arguments
#define MAX_COMMAND_LINE_LENGTH 256

ArgumentParser::ArgRecord *
WarpedGem5Main::getArgumentList( WarpedGem5Main &main ) {
  static ArgumentParser::ArgRecord args[] = {
    {"-configuration", "specify configuration file", &main.configurationFileName,
     ArgumentParser::STRING, false},
    {"-simulateUntil", "specify a simulation end time", &main.simulateUntil,
     ArgumentParser::STRING, false},
    {"-debug", "display debug messages", &main.debugFlag, 
     ArgumentParser::BOOLEAN, false},
    {"", "", 0, ArgumentParser::BOOLEAN, false}
  };
    
  return args;
};

WarpedGem5Main::WarpedGem5Main( Application *initApplication ) :
  errors( 0 ),
  warnings( 0 ),
  configurationFileName( "" ),
  debugFlag( false ),
  simulateUntil( "" ),
  myApplication( initApplication ),
  mySimulation( 0 ){}

WarpedGem5Main::~WarpedGem5Main()
{
	delete mySimulation;
	delete myApplication;
}

void
WarpedGem5Main::displayParameters( string executableName ){
  ArgumentParser ap( getArgumentList( *this ) );
  ap.printUsage( executableName, cerr );
}

bool
WarpedGem5Main::checkConfigFile( string configFileName ){
  string choice;
  if( configFileName == "" ){
    cerr << "A Simulation Configuration File has not been specified" << endl;
    cerr << "Shall I create a default configuration: [y/n]: ";
    cin >> choice;
    // we are going to uppercase whatever the choice is ...
    string upChoice = choice;
    std::transform(upChoice.begin(), upChoice.end(), upChoice.begin(),(int(*)(int)) std::toupper);
            
    if( upChoice == "Y" ){
      cerr << "Creating default configuration file: simulation.conf"
	   << endl;
      cerr << "This has not been implemented yet ..." << endl;
      exit(-1);
    }
  }
  return true;
}

void
WarpedGem5Main::registerKernelDeserializers(){
  CirculateInitializationMessage::registerDeserializer();
  EventMessage::registerDeserializer();
  GVTUpdateMessage::registerDeserializer();
  InitializationMessage::registerDeserializer();
  UsefulWorkMessage::registerDeserializer();
  IntVTime::registerDeserializer();
  MatternGVTMessage::registerDeserializer();
  NegativeEvent::registerDeserializer();
  NegativeEventMessage::registerDeserializer();
  RestoreCkptMessage::registerDeserializer();
  StartMessage::registerDeserializer();
  TerminateToken::registerDeserializer();
}

vector<string>
WarpedGem5Main::buildArgumentVector( int argc, char **argv ){
  vector<string> retval;
  for( int i = 0; i < argc; i++ ){
    retval.push_back( string( argv[i] ) );
  }
  return retval;
}

SimulationConfiguration *
WarpedGem5Main::readConfiguration( const string &configurationFileName,
			       const vector<string> &argumentVector ){
  SimulationConfiguration *configuration = 0;
  if( configurationFileName != "" ){
    if( checkConfigFile( configurationFileName ) == false ){
      cerr << "Can't read configuration file " << configurationFileName << endl;
      exit( -1 );
    }
    configuration = SimulationConfiguration::parseConfiguration( configurationFileName,
								 argumentVector );
    if( configuration == 0 ){
      cerr << "There was a problem parsing configuration " << configurationFileName
	   << ", exiting." << endl;
      exit( -1 );
    } else {
      cerr << "Using configuration file: " << configurationFileName << endl;
    }
  }
  return configuration;
}

void
WarpedGem5Main::initializeSimulation( vector<string> &commandLineArgs ){
  registerKernelDeserializers();
  myApplication->registerDeserializers();

  // need to save this this for TCPSelect before ArgumentParser cannibalizes it.
  // TCPSelect uses it to build the argument list for ssh when launching 
  // "slave" simulation managers
  vector<string> argsCopy(commandLineArgs);

  ArgumentParser ap( getArgumentList( *this ));
  ap.checkArgs( commandLineArgs, false );

  if( debugFlag == true ){
    utils::enableDebug();
    utils::debug << "Debug output enabled with -debug" << endl;
  }

  SimulationConfiguration *configuration =
          readConfiguration( configurationFileName, argsCopy );

  if( configuration != 0 ){
    Spinner::spinIfRequested( "SpinBeforeConfiguration", *configuration );
  }
  // We have to let the application initialize before we can do much else.
  myApplication->initialize( commandLineArgs );

  // else, configuration is NULL, and we'll run with a default configuration
  mySimulation = Simulation::instance( configuration, myApplication );
  mySimulation->initialize();


  delete configuration;
}

void
WarpedGem5Main::simulate( const VTime &simulateUntil ){
  mySimulation->simulate( simulateUntil );
}

bool
WarpedGem5Main::simulationComplete( ){
  return mySimulation->simulationComplete();
}

void
WarpedGem5Main::finalize(){
  mySimulation->finalize();
}

const VTime &
WarpedGem5Main::getCommittedTime(){
  return mySimulation->getCommittedTime();
}

const VTime &
WarpedGem5Main::getNextEventTime(){
  return mySimulation->getNextEventTime();
}


int 
WarpedGem5Main::main( int argc, char **argv ){
  // Setup the Warped components
  vector<string> args = buildArgumentVector( argc, argv );
  initializeSimulation( args );
  
  // Setup the gem5 components
  m5Main( argc, argv );
    
  if (simulateUntil == "") {
    simulate( myApplication->getPositiveInfinity() );
  }
  else {
    simulate( myApplication->getTime(simulateUntil));
  }

  finalize();

  return errors;
}
