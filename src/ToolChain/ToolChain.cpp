#include "ToolChain.h"

ToolChain::ToolChain(std::string configfile,  int argc, char* argv[]){
  
  m_data.vars.Initialise(configfile);

  Store config;
  config.Initialise(configfile);
  config.Get("verbose",m_verbose);
  config.Get("error_level",m_errorlevel);
  config.Get("attempt_recover",m_recover);
  config.Get("log_mode",m_log_mode);
  config.Get("log_local_path",m_log_local_path);
 
  m_data.vars.Set("argc",argc);

  for(int i=0; i<argc ; i++){

    std::stringstream tmp;
    tmp<<"$"<<i;
    m_data.vars.Set(tmp.str(),argv[i]);

  }

  
  Init();

  Inline=0;
  interactive=false;
  config.Get("Inline",Inline);
  config.Get("Interactive",interactive);

  bool sendflag=true;
  bool receiveflag=true;
                                                                                      
 logmessage<<yellow<<"********************************************************"<<std::endl<<"**** Tool chain created ****"<<std::endl<<"********************************************************"<<plain<<std::endl;

  m_data.Log->Log(logmessage.str(),1,m_verbose);
  logmessage.str("");


  std::string toolsfile="";
  config.Get("Tools_File",toolsfile);
  
  if(toolsfile!=""){
    std::ifstream file(toolsfile.c_str());
    std::string line;
    if(file.is_open()){

      while (getline(file,line)){
        if (line.size()>0){
          if (line.at(0)=='#')continue;
	  std::string name;
	  std::string tool;
	  std::string conf;
	  std::stringstream stream(line);

          if(stream>>name>>tool>>conf) Add(name,Factory(tool),conf);

        }
      }
    }
    
    file.close();
    
  }
  
  if(Inline==-1){
    bool StopLoop=false;
    m_data.vars.Set("StopLoop",StopLoop);
    Initialise();
    while(!StopLoop){
      Execute();
      m_data.vars.Get("StopLoop",StopLoop);
    }
    Finalise();
    
  }
  
  else if(Inline>0){
    Initialise();
    Execute(Inline);
    Finalise();
  }
  
  else if(interactive) Interactive();
  
  
}

ToolChain::ToolChain(int verbose, int errorlevel, std::string logmode, std::string log_local_path){
  
  m_verbose=verbose;
  m_errorlevel=errorlevel;
  m_log_mode = logmode;
  m_log_local_path=log_local_path;
  Init();

  
}

void ToolChain::Init(){

  if(m_log_mode!="Off"){
 
  bcout=std::cout.rdbuf();
  out=new  std::ostream(bcout);
  }
  m_data.Log= new Logging(*out,m_log_mode, m_log_local_path);

  if(m_log_mode!="Off") std::cout.rdbuf(&(m_data.Log->buffer));
  
  execounter=0;
  Initialised=false;
  Finalised=true;
  paused=false;

}



void ToolChain::Add(std::string name,Tool *tool,std::string configfile){
  if(tool!=0){

    logmessage<<cyan<<"Adding Tool='"<<name<<"' to ToolChain"<<plain;
    m_data.Log->Log(logmessage.str(),1,m_verbose);
    logmessage.str("");
    
    m_tools.push_back(tool);
    m_toolnames.push_back(name);
    m_configfiles.push_back(configfile);
    
    logmessage<<green<<"Tool='"<<name<<"' added successfully"<<plain<<std::endl;
    m_data.Log->Log(logmessage.str(),1,m_verbose);
    logmessage.str("");
    
  }
  else{
    logmessage<<red<<"WARNING!!! Tool='"<<name<<"' Does Not Exist in factory!!! "<<plain<<std::endl;
    m_data.Log->Log(logmessage.str(),0,m_verbose);
    logmessage.str("");
  }

}



int ToolChain::Initialise(){

  bool result=0;

  if (Finalised){
    logmessage<<yellow<<"********************************************************"<<std::endl<<"**** Initialising tools in toolchain ****"<<std::endl<<"********************************************************"<<plain<<std::endl;
    m_data.Log->Log(logmessage.str(),1,m_verbose);
    logmessage.str("");
  
    for(int i=0 ; i<m_tools.size();i++){  
      
      logmessage<<cyan<<"Initialising "<<m_toolnames.at(i)<<plain;
      m_data.Log->Log(logmessage.str(),2,m_verbose);
      logmessage.str("");

#ifndef DEBUG      
      try{ 
#endif   

	if(m_tools.at(i)->Initialise(m_configfiles.at(i), m_data)){
	  logmessage<<green<<m_toolnames.at(i)<<" initialised successfully"<<plain<<std::endl;
	  m_data.Log->Log( logmessage.str(),2,m_verbose);
	  logmessage.str("");
	}
	else{
	  logmessage<<red<<"WARNING !!!!! "<<m_toolnames.at(i)<<" Failed to initialise (exit error code)"<<plain<<std::endl;
          m_data.Log->Log( logmessage.str(),0,m_verbose);
          logmessage.str("");
	  result=1;
	  if(m_errorlevel>1) exit(1);
         }

#ifndef DEBUG	
    }      
      catch(...){
	logmessage<<red<<"WARNING !!!!! "<<m_toolnames.at(i)<<" Failed to initialise (uncaught error)"<<plain<<std::endl;
	m_data.Log->Log( logmessage.str(),0,m_verbose);
	logmessage.str("");
	result=2;
	if(m_errorlevel>0) exit(1);
       }
#endif
     
    }
      
    logmessage<<std::endl<<yellow<<"**** Tool chain initialised ****"<<std::endl<<"********************************************************"<<plain<<std::endl;
    m_data.Log->Log( logmessage.str(),1,m_verbose);
    logmessage.str("");

    execounter=0;
    Initialised=true;
    Finalised=false;
    }
  else {
    logmessage<<purple<<"********************************************************"<<std::endl<<std::endl<<" ERROR: ToolChain Cannot Be Initialised as already running. Finalise old chain first"<<"********************************************************"<<plain<<std::endl;
    m_data.Log->Log( logmessage.str(),0,m_verbose);
    logmessage.str("");


    result=-1;
  }
  
  return result;
}



int ToolChain::Execute(int repeates){
 
  int result =0;
  
  if(Initialised){

    if(Inline){
      logmessage<<yellow<<"********************************************************"<<std::endl<<"**** Executing toolchain "<<repeates<<" times ****"<<std::endl<<"********************************************************"<<plain<<std::endl;
    m_data.Log->Log( logmessage.str(),2,m_verbose);
    logmessage.str("");
    }

    for(int i=0;i<repeates;i++){
       logmessage<<yellow<<"********************************************************"<<std::endl<<"**** Executing tools in toolchain ****"<<std::endl<<"********************************************************"<<plain<<std::endl;
      m_data.Log->Log( logmessage.str(),3,m_verbose);
      logmessage.str("");
      
      for(int i=0 ; i<m_tools.size();i++){
	
	logmessage<<cyan<<"Executing "<<m_toolnames.at(i)<<plain;
	m_data.Log->Log( logmessage.str(),4,m_verbose);
	logmessage.str("");	
	
#ifndef DEBUG
	try{
#endif

	  if(m_tools.at(i)->Execute()){
	    logmessage<<green<<m_toolnames.at(i)<<" executed  successfully"<<plain<<std::endl;
	    m_data.Log->Log( logmessage.str(),4,m_verbose);
	    logmessage.str("");
	    
	  }
	  else{
	    logmessage<<red<<"WARNING !!!!!! "<<m_toolnames.at(i)<<" Failed to execute (error code)"<<plain<<std::endl;
            m_data.Log->Log( logmessage.str(),0,m_verbose);
            logmessage.str("");
	    
	    
	    result=1;
	    if(m_errorlevel>1){
	      if(m_recover){
		m_errorlevel=0;
		Finalise();
	      }
	      exit(1);
	    }
	  }  

#ifndef DEBUG
	}
	
	catch(...){
	  logmessage<<red<<"WARNING !!!!!! "<<m_toolnames.at(i)<<" Failed to execute (uncaught error)"<<plain<<std::endl;
	  m_data.Log->Log( logmessage.str(),0,m_verbose);
	  logmessage.str("");
	  
	  result=2;
	  if(m_errorlevel>0){
	    if(m_recover){
		m_errorlevel=0;
		Finalise();
	    }
	    exit(1);
	  }
	}
#endif	

      }
      logmessage<<yellow<<"**** Tool chain executed ****"<<std::endl<<"********************************************************"<<plain<<std::endl;
      m_data.Log->Log( logmessage.str(),3,m_verbose);
      logmessage.str("");
    }
    
    execounter++;
    if(Inline){
      logmessage<<yellow<<"********************************************************"<<std::endl<<"**** Executed toolchain "<<repeates<<" times ****"<<std::endl<<"********************************************************"<<plain<<std::endl;
      m_data.Log->Log( logmessage.str(),2,m_verbose);
      logmessage.str("");
    }
  }
  
  else {
    logmessage<<purple<<"********************************************************"<<std::endl<<std::endl<<" ERROR: ToolChain Cannot Be Executed As Has Not Been Initialised yet."<<std::endl<<"********************************************************"<<plain<<std::endl;
    m_data.Log->Log( logmessage.str(),0,m_verbose);
    logmessage.str("");
    result=-1;
  }

  return result;
}



int ToolChain::Finalise(){
 
  int result=0;

  if(Initialised){
    logmessage<<yellow<<"********************************************************"<<std::endl<<"**** Finalising tools in toolchain ****"<<std::endl<<"********************************************************"<<plain<<std::endl;
    m_data.Log->Log( logmessage.str(),1,m_verbose);
    logmessage.str("");

    for(int i=0 ; i<m_tools.size();i++){
      
      logmessage<<cyan<<"Finalising "<<m_toolnames.at(i)<<plain;
      m_data.Log->Log( logmessage.str(),2,m_verbose);
      logmessage.str("");

#ifndef DEBUG
      try{
#endif

	if(m_tools.at(i)->Finalise()){
	  logmessage<<green<<m_toolnames.at(i)<<" Finalised successfully"<<plain<<std::endl;
	  m_data.Log->Log( logmessage.str(),2,m_verbose);
	  logmessage.str("");

	}
	else{
	  logmessage<<red<<"WARNING !!!!!!! "<<m_toolnames.at(i)<<" Finalised successfully (error code)"<<plain<<std::endl;
	  m_data.Log->Log( logmessage.str(),0,m_verbose);
	  logmessage.str("");
	  
	  result=1;
	  if(m_errorlevel>1)exit(1);
	}  

#ifndef DEBUG      
      }
      
      catch(...){
	logmessage<<red<<"WARNING !!!!!!! "<<m_toolnames.at(i)<<" Finalised successfully (uncaught error)"<<plain<<std::endl;
	m_data.Log->Log( logmessage.str(),0,m_verbose);
	logmessage.str("");
	
	result=2;
	if(m_errorlevel>0)exit(1);
      }
#endif

    }
    logmessage<<yellow<<"**** Toolchain Finalised ****"<<std::endl<<"********************************************************"<<plain<<std::endl;
    m_data.Log->Log( logmessage.str(),1,m_verbose);
    logmessage.str("");
    
    execounter=0;
    Initialised=false;
    Finalised=true;
    paused=false;
  }
  
  else { 
    logmessage<<purple<<"********************************************************"<<std::endl<<std::endl<<" ERROR: ToolChain Cannot Be Finalised As Has Not Been Initialised yet."<<std::endl<<"********************************************************"<<plain<<std::endl;
    m_data.Log->Log( logmessage.str(),0,m_verbose);
    logmessage.str("");

    result=-1;
  }



  return result;
}


void ToolChain::Interactive(){
  exeloop=false;
   
  ToolChainargs* args=new ToolChainargs;

  args->msgflag=&msgflag;
  args->command="";

  pthread_create (&thread[0], NULL, ToolChain::InteractiveThread, args);

  bool running=true; 
  
  msgflag=false;
 
  while (running){
    
    if(msgflag){  
      logmessage<<ExecuteCommand(args->command);
      printf("%s \n\n",logmessage.str().c_str());
      logmessage.str("");
      if(args->command=="Quit"){
	running=false;
      }
      
      args->command="";
      
      logmessage<<"Please type command : "<<cyan<<"Start, Pause, Unpause, Stop, Status, Quit, ?, (Initialise, Execute, Finalise)"<<plain;
      printf("%s \n %s",logmessage.str().c_str(),">");
      logmessage.str("");
      msgflag=false;
      
    }
    
    ExecuteCommand("");
  }
  
  pthread_join(thread[0], NULL);
  delete args;
  args=0;
  
}  



std::string ToolChain::ExecuteCommand(std::string command){
  std::stringstream returnmsg;
  
  if(command!=""){
    if(command=="Initialise"){
      int ret=Initialise();
      if (ret==0)returnmsg<<"Initialising ToolChain";
      else returnmsg<<red<<"Error Code "<<ret<<plain;
    }
    else if (command=="Execute"){
      int ret=Execute();
      if (ret==0)returnmsg<<"Executing ToolChain";
      else returnmsg<<red<<"Error Code "<<ret<<plain;
    }
    else if (command=="Finalise"){
      exeloop=false;
      int ret=Finalise();
      if (ret==0)returnmsg<<"Finalising  ToolChain";
      else returnmsg<<red<<"Error Code "<<ret<<plain;
    }
    else if (command=="Quit"){
      returnmsg<<"Quitting";
    }
    else if (command=="Start"){
      int ret=Initialise();
      exeloop=true;
      execounter=0;
      if (ret==0)returnmsg<<"Starting ToolChain";
      else returnmsg<<red<<"Error Code "<<ret<<plain;
    }
    else if(command=="Pause"){
      exeloop=false;
      paused=true;
      returnmsg<<"Pausing ToolChain";
    }
    else if(command=="Unpause"){
      exeloop=true;
      paused=false;
      returnmsg<<"Unpausing ToolChain";
    }
    else if(command=="Stop") {
      exeloop=false;
      int ret=Finalise();
      if (ret==0)returnmsg<<"Stopping ToolChain";
      else returnmsg<<red<<"Error Code "<<ret<<plain;
    }
    
    else if(command=="Restart") {
      int ret=Finalise()+Initialise();
      if (ret==0)returnmsg<<"Restarting ToolChain";
      else returnmsg<<red<<"Error Code "<<ret<<plain;
    }
    
    else if(command=="Status"){
      std::stringstream tmp;
      if(Finalised) tmp<<"Waiting to Initialise ToolChain";
      if(Initialised && execounter==0) tmp<<"Initialised waiting to Execute ToolChain";
      if(Initialised && execounter>0){
	if(paused)tmp<<"ToolChain execution pasued";
	else tmp<<"ToolChain running (loop counter="<<execounter<<")";
      }
      
      if(*(m_data.vars["Status"])!="") tmp<<" : "<<*(m_data.vars["Status"]);
      returnmsg<<tmp.str();
    }
    else if(command=="?")returnmsg<<" Available commands: Initialise, Execute, Finalise, Start, Stop, Restart, Pause, Unpause, Quit, Status, ?";
    else if(command!=""){
      returnmsg<<purple<<"command not recognised please try again"<<plain;
    }
  }
  if(Finalised || (!Finalised && !exeloop)) usleep(100);
  if(exeloop) Execute();
  return returnmsg.str();
}


void* ToolChain::InteractiveThread(void* arg){

  ToolChainargs* args=static_cast<ToolChainargs*>(arg);

  bool running=true;
  
  printf("%s %s %s %s\n %s %s %s","Please type command :",cyan," Start, Pause, Unpause, Stop, Restart, Status, Quit, ?, (Initialise, Execute, Finalise)",plain,green,">",plain);

  while (running){
    
    std::cin>>args->command;
    *(args->msgflag)=true;
    if (args->command=="Quit")running=false;
  }
  
  pthread_exit(NULL);
  
}


ToolChain::~ToolChain(){
  
  for (int i=0;i<m_tools.size();i++){
    delete m_tools.at(i);
    m_tools.at(i)=0;
  }
  
  m_tools.clear();
  

  delete m_data.Log;  
  m_data.Log=0;

  if(m_log_mode!="Off"){
    std::cout.rdbuf(bcout);
    delete out;
    out=0;
  }  
  
  
  
}

