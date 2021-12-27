#ifndef TOOLCHAIN_H
#define TOOLCHAIN_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> 
#include <unistd.h>

#include "Tool.h"
#include "DataModel.h"
#include "Logging.h"
#include "Factory.h"
#include "Store.h"

/**
 * \struct ToolChainargs
 *
 * Simple struct to pass thread initalisation variables to Interactive ToolChain thread
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
* Contact: b.richards@qmul.ac.uk
*/
struct ToolChainargs{

  ToolChainargs(){};

  bool *msgflag; ///< Message flag used to indiacte if a new interactive command has been submitted to the interactive thread and needs execution on the main thread. 
  std::string command;

};

/**
 * \class ToolChain
 *
 * This class holds a dynamic list of Tools which can be Initialised, Executed and Finalised to perform program operation. Large number of options in terms of run modes and setting can be assigned.
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
* Contact: b.richards@qmul.ac.uk
*/
class ToolChain{
  
 public:
  ToolChain(std::string configfile, int argc=0, char* argv[]=0); ///< Constructor that obtains all of the configuration varaibles from an input file. @param configfile The path and name of the config file to read configuration values from.

  /**
     Constructor with explicit configuration variables passed as arguments.
     @param verbose The verbosity level of the ToolChain. The higher the number the more explicit the print outs. 0 = silent apart from errors.
     @param errorlevel The behavior that occurs when the ToolChain encounters an error. 0 = do not exit for both handeled and unhandeled errors, 1 = exit on unhandeled errors only, 2 = exit on handeled and unhandeled errors.
     @param service The name of the ToolChain service to boradcast for service discovery
     @param logmode Where log printouts should be forwarded too. "Interactive" = cout, "Remote" = send to a remote logging system, "local" = ouput logs to a file. 
     @param log_local_path The file path and name of where to store logs if in Local logging mode.
     @param log_service Service name of remote logging system if in Remote logging mode.
     @param log_port The Port number of the remote logging service
     @param pub_sec The number of seconds between publishing multicast serivce beacons
     @param kick_sec The number of seconds to wait before removing a servic from the remote services list if no beacon received.
     @param IO_Threads The number of ZMQ IO threads to use (~1 per Gbps of traffic).
   */
  ToolChain(int verbose=1, int errorlevel=0, std::string logmode="Off", std::string log_local_path="./log"); 
  //verbosity: true= print out status messages , false= print only error messages;
  //errorlevels: 0= do not exit; error 1= exit if unhandeled error ; exit 2= exit on handeled and unhandeled errors; 
  virtual ~ToolChain(); 
  bool Add(std::string name,Tool *tool,std::string configfile=""); ///< Add a Tool to the ToolChain. @param name The name used in logs when reffering to the Tool. @param tool A pointer to the tool to be added to the ToolChain. @param configfile The configuration file path and name to be passed to the Tool. 
  int Initialise(); ///< Initialise all Tools in the ToolChain sequentially.
  int Execute(int repeates=1); ///< Execute all Tools in the ToolChain sequentially. @param repeates How many times to run sequential Execute loop.
  int Finalise(); ///< Finalise all Tools in the ToolCahin sequentially.
  void Interactive(); ///< Start interactive thread to accept commands and run ToolChain in interactive mode.
  DataModel m_data; ///< Direct access to transient data model class of the Tools in the ToolChain. This allows direct initialisation and copying of variables.

  protected:

  virtual void Init();
  void Inline();
  bool LoadTools(std::string filename);

  static  void *InteractiveThread(void* arg);
  std::string ExecuteCommand(std::string connand);


  //Tools configs and data
  std::vector<Tool*> m_tools;
  std::vector<std::string> m_toolnames;
  std::vector<std::string> m_configfiles;
  //DataModel m_data;
  std::streambuf  *bcout;
  std::ostream *out;
  
  //conf variables
  int m_verbose;
  int m_errorlevel;
  std::string m_log_mode;
  std::string m_log_local_path;
  bool m_interactive;
  int m_inline;
  bool m_recover;
  
  //status variables
  bool exeloop;
  unsigned long execounter;
  bool Initialised;
  bool Finalised;
  bool paused;
  Logging* m_log;

  //socket coms and threading variables
  pthread_t thread[1];
  bool msgflag;

};

#endif
