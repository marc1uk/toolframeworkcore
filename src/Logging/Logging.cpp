#include "Logging.h"

Logging::MyStreamBuf::~MyStreamBuf(){

  if(m_mode=="Local")file.close();

}

Logging::MyStreamBuf::MyStreamBuf (std::ostream& str, std::string mode, std::string localpath):output(str){

  m_mode=mode;
  m_messagelevel=1;
  m_verbose=1;

  if(m_mode=="Local"){

    file.open(localpath.c_str());
    psbuf = file.rdbuf();
    output.rdbuf(psbuf);
    
  }


}


int Logging::MyStreamBuf::sync ( )
{
  if(m_messagelevel <= m_verbose){


    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);
    std::string t(buffer);

    
    if(m_mode=="Local"){
      //output<< "{"<<t<<"} ["<<m_messagelevel<<"]: " << str();
      output<<str();
      str("");
      output.flush();
      
    }
    else if(m_mode=="Interactive"){
      std::string code="";
      /*      if(m_messagelevel ==0) code="231";
	      else if (m_messagelevel ==1) code="174";
	      else if (m_messagelevel ==2) code="129";
	      else if (m_messagelevel ==3) code="154";
	      else if (m_messagelevel ==4) code="159";
	      else if (m_messagelevel ==5) code="125";
	      else if (m_messagelevel ==6) code="21";
	      else if (m_messagelevel ==7) code="46";
	      else if (m_messagelevel ==8) code="51";
	      else if (m_messagelevel ==9) code="93";
	      else if (m_messagelevel ==10) code="208";
	      else if (m_messagelevel ==10) code="226";
	      else if (m_messagelevel >=12) code="201";
	      
	      
	      output<<"\033[38;5;"<<code<<"m["<<m_messagelevel<<"]: " << str()<<"\033[0m";
     */ 
      //output<<"["<<m_messagelevel<<"]: " << str();
      output<<str();
      str("");
      output.flush(); 
    }
  }
  else{
    str("");
  }
  
  return 0;
}

bool Logging::MyStreamBuf::ChangeOutFile(std::string localpath){
  
  if(m_mode=="Local"){
    file.close();
    file.open(localpath.c_str());
    psbuf = file.rdbuf();
    output.rdbuf(psbuf);
  }
  return true;  
}

