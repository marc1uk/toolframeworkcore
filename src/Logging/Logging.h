#ifndef LOGGING_H
#define LOGGING_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <map>

#include <pthread.h>
#include <time.h>

#include "Store.h"

#define red "\033[38;5;196m"
#define darkred "\033[38;5;88m"
#define green "\033[38;5;46m"
#define darkgreen "\033[38;5;22m"
#define blue "\033[38;5;21m"
#define darkblue "\033[38;5;18m"
#define yellow "\033[38;5;226m"
#define darkyellow "\033[38;5;142m"
#define orange "\033[38;5;208m"
#define darkorange "\033[38;5;130m"
#define pink "\033[38;5;201m"
#define darkpink "\033[38;5;129m"
#define purple "\033[38;5;57m"
#define darkpurple "\033[38;5;54m"
#define cyan  "\033[38;5;51m"
#define darkcyan  "\033[38;5;39m" 
#define white "\033[38;5;255m"
#define gray "\033[38;5;243m"
#define plain "\033[0m"

/**
 * \struct MsgL
 *
 *This struct variables for setting the message level of streamed messages to the logging class
 *
 *
 * $Author: B.Richards $
 * $Date: 2020/12/30 12:50:00 $
 * Contact: b.richards@qmul.ac.uk
 */

struct MsgL{

  MsgL(int in_messagelevel, int in_verbose){
    messagelevel=in_messagelevel;
    verbose=in_verbose;
  }
  MsgL ML(int in_messagelevel) {
    messagelevel=in_messagelevel;
    return *this;
  }
  
  int messagelevel; ///< Message level paramiter
  int verbose; ///< Verbosity level pramiter
  
};


/**
 * \class Logging
 *
 *This class handels the logging, which can be directed to screen or file or over the via the ToolChain Config file
 *
 *
 * $Author: B.Richards $
 * $Date: 2019/05/27 18:34:00 $
 * Contact: b.richards@qmul.ac.uk
 */


class Logging: public std::ostream {

  class MyStreamBuf: public std::stringbuf
    {

      std::ostream&   output;
      
    public:
      MyStreamBuf(std::ostream& str ,std::string mode, std::string localpath="");
      
      virtual int sync ( );
      
      bool ChangeOutFile(std::string localpath);

      int m_messagelevel;
      int m_verbose;
      
      ~MyStreamBuf();

    private:

      

      std::string m_mode;

      std::ofstream file;
      std::streambuf *psbuf, *backup;

   
    }; 

 public:
  
  MyStreamBuf buffer; ///< Stream buffer used to replace std::cout for redirection to coustom output.
  
  /**
     Constructor for Logging class
     
     @param str
     @param context Pointer to ZMW context used for creating sockets
     @param UUID ToolChain UUID for unique labelling of log messages
     @param service 
     @param mode
     @param localpath Local path for log output file 
     @param logservice Remote service to connect to to send logs
     @param logport remothe port to send logging information to
  */
 Logging(std::ostream& str, std::string mode, std::string localpath=""):std::ostream(&buffer), buffer(str, mode, localpath){};

  
  /**
       Function to create a log messages. 
 
       @param message templated log message text.
       @param messagelevel message verbosity level of the message being sent (e.g. if 'messagelevel>= verbose' Then message is sent). 
       @param verbose verbosity level of the current Tool.    
       
  */
  template <typename T>  void Log(T message, int messagelevel=1, int verbose=1){
    if(messagelevel<=verbose){    
      std::stringstream tmp;
      tmp<<message;
      buffer.m_messagelevel=messagelevel;
      buffer.m_verbose=verbose;
      std::cout<<tmp.str()<<plain<<std::endl;
      buffer.m_messagelevel=1;
      buffer.m_verbose=1;
    } 
  }
  
  
  /**
     Functionn to change the logs out file if set to a local path.
     
     @param localpath path to new log file.
     @return value is bool success of opening new logfile.
     
  */
  bool ChangeOutFile(std::string localpath){return buffer.ChangeOutFile(localpath);} 
  
  


  Logging& operator<<(MsgL a){

    buffer.m_messagelevel=a.messagelevel;
    buffer.m_verbose=a.verbose;

    return *this;
  }
  
  Logging& operator<<(std::ostream& (*foo)(std::ostream&)) { 
    
    std::cout<<plain<<std::endl;
    
  }

  template<typename T>  Logging& operator<<(T &a){
    
    std::stringstream tmp; 
    tmp<<a;
  
    std::cout<<tmp.str();
    return *this;
    
  }
  
 private:
  

 };


#endif
