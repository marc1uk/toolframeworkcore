#include <BStore.h>

BStore::BStore(bool header, bool type_checking){
  //  m_serialise=true;
  m_type_checking=type_checking;
  m_has_header=header;  
  if(header) m_header=new BStore(false,false);
  else m_header=0;
}

std::string BStore::GetVersion(){return "BStore:1.0.0";}

bool BStore::GetFlags(unsigned int file_end){


  m_file_end=file_end;       // not sure if i wnna keep thiese two here
  m_open_file_end=file_end;
  
  if(!output.Bseek( file_end-(sizeof(m_header_start)+sizeof(m_has_header)+sizeof(m_lookup_start)+sizeof(m_lookup_size)+sizeof(m_type_checking)+sizeof(m_type)+sizeof(m_previous_file_end)), SEEK_SET)) return false;
  //  std::cout<<"current pos-11="<<output.Btell()<<std::endl;
  m_flags_start=output.Btell();
  //std::cout<<"flast start pos="<<m_flags_start<<std::endl;
  //std::cout<<"current pos5="<<output.Btell()<<std::endl;
  if(!(output >> m_header_start)) return false;                                               
  if(!(output >> m_has_header)) return false;
  if(!(output >> m_lookup_start)) return false;
  //std::cout<<"current pos="<<output.Btell()<<std::endl;
  if(!(output >> m_lookup_size)) return false;
  //std::cout<<"current pos="<<output.Btell()<<std::endl;
  if(!(output >> m_type_checking)) return false;
  //std::cout<<"current pos="<<output.Btell()<<std::endl;
  if(!(output >> m_type)) return false; 
  //std::cout<<"current pos="<<output.Btell()<<std::endl;
  if(!(output >> m_previous_file_end)) return false;
  //std::cout<<"current pos="<<output.Btell()<<std::endl;
  //std::cout<<"loading"<<std::endl;
  //std::cout<<"m_lookup_start="<< m_lookup_start<<std::endl;
  //std::cout<<"m_lookup_size="<< m_lookup_size<<std::endl;
  //std::cout<<"m_type_checking="<< m_type_checking<<std::endl;
  //std::cout<<"m_type="<< m_type<<std::endl;
  //std::cout<<"m_previous_file_end="<< m_previous_file_end<<std::endl;


  return true;
}

bool BStore::GetFlags(std::string filename, unsigned int file_end){
  
  if(!output.Bclose()) return false;
  if(!output.Bopen(filename, READ, UNCOMPRESSED)) return false;
  if(!output.Bseek(0, SEEK_END)) return false;
  if(file_end==0){
    if(!output.Bseek(0, SEEK_END)) return false;
    m_file_end=output.Btell();
  }
  else m_file_end=file_end;
  m_open_file_end=m_file_end;
  //std::cout<<"flile end="<<m_file_end<<std::endl;
  m_file_name=filename;
  //std::cout<<"current pos-2="<<output.Btell()<<std::endl;
  
  if(!GetFlags(m_file_end)) return false;

  //  if(!output.Bseek(0, SEEK_END)) return false;
  //std::cout<<"current pos="<<output.Btell()<<std::endl;
  
  if(!output.Bclose()) return false;
  //  m_entry=m_lookup_size;
  
  return true;
}



bool BStore::Initnew(std::string filename, enum_type type, bool header, bool type_checking, unsigned int file_end){
  struct stat buffer;   
  if(stat (filename.c_str(), &buffer) == 0){ //if file exists    
    //std::cout<<"file exists"<<std::endl;
    if(!GetFlags(filename, file_end)) return false;

    if(type==uncompressed){
      if(!output.Bopen(filename, READ_APPEND, UNCOMPRESSED)) return false;
    }
#ifdef ZLIB
    else if(type==compressed){
      if(!output.Bopen(filename, READ, COMPRESSED)) return false;
    }
    else if(type==post_pre_compress){
      //std::cout<<"BEN loading file correctly"<<std::endl;
      if(!output.Bopen(filename, READ_APPEND, POST_PRE_COMPRESS)) return false;
      if(!output.Bseek(0, SEEK_END)) return false;
      m_file_end=output.Btell();
      m_open_file_end=output.Btell();
    }
#endif
    else return false;
    
    if(!output.Bseek(m_lookup_start,SEEK_SET)) return false;
    if(!(output >> m_lookup)) return false;
    if(!GetHeader()) return false;  
    if(!GetEntry(0)) return false;
    
  }
  
  else{ 
    //std::cout<<"file doesnt exist"<<std::endl;
    m_file_end=0;
    //m_entry=0;
    if(type==uncompressed){
      if(!output.Bopen(filename, READ_APPEND, UNCOMPRESSED)) return false;
    }
#ifdef ZLIB
    else if(type==compressed){
      if(!output.Bopen(filename, APPEND, COMPRESSED)) return false;
    }
    else if(type==post_pre_compress){
      if(!output.Bopen(filename, READ_APPEND, POST_PRE_COMPRESS)) return false;
    }
#endif
    else return false;

    if(m_header!=0){
      delete m_header;
      m_header=0;
    }
    m_has_header=header;
    if(header) m_header= new BStore(false,false);

    m_type=type;
    m_type_checking=type_checking;
    m_file_name=filename;    
  }

  m_update=false;
  return true;
}

/*
void BStore::Init(){
  //    file = gzopen ("myfile.bin", "ab");
  FILE * pFile;
  pFile = fopen ("myfile.bin", "rb");
  lseek(fileno(pFile), 0,  SEEK_END);
  //std::cout<<ftell(pFile)<<std::endl;
  file_end=ftell(pFile);
  lseek(fileno(pFile), 0-(sizeof(lookup_start)+sizeof(lookup_size)+sizeof(type)+sizeof(previous_file_end)),  SEEK_CUR);
  flags_start=ftell(pFile);
  fread(&lookup_start, sizeof(lookup_start),1, pFile);
  fread(&lookup_size, sizeof(lookup_size),1, pFile);
  fread(&type, sizeof(type),1, pFile);
  fread(&previous_file_end, sizeof(previous_file_end),1, pFile);
  fclose(pFile); 
  
  entry=0;
  file = gzopen ("myfile.bin", "wb");
  update=false;
  previous_file_end=0;
  m_type_checking=false;
}

void BStore::Init2(){

  FILE * pFile;
  pFile = fopen ("myfile.bin", "rb");
  lseek(fileno(pFile), 0,  SEEK_END);
  //std::cout<<ftell(pFile)<<std::endl;
  file_end=ftell(pFile);
  lseek(fileno(pFile), 0-(sizeof(lookup_start)+sizeof(lookup_size)+sizeof(type)+sizeof(previous_file_end)),  SEEK_CUR);
  flags_start=ftell(pFile);
  fread(&lookup_start, sizeof(lookup_start),1, pFile);
  fread(&lookup_size, sizeof(lookup_size),1, pFile);
  fread(&type, sizeof(type),1, pFile);
  fread(&previous_file_end, sizeof(previous_file_end),1, pFile);
  //    std::cout<<"lookup_start="<<lookup_start<<std::endl;
  //std::cout<<"lookup_size="<<lookup_size<<std::endl;
  fclose(pFile);
  
  entry=0;
  file = gzopen ("myfile.bin", "rb");
  //    gzbuffer(file, 128000);
  
  gzseek(file,lookup_start,SEEK_SET);
  
  for(unsigned int i=0;i<lookup_size; i++){
    unsigned int key;
    if(!gzread(file, &key, sizeof(key))) break;
    unsigned int value;
    if(!gzread(file, &value, sizeof(value))) break;
    lookup[key]=value;
    
  }
  update=false;
  m_type_checking=false;  
}

*/
bool BStore::Save(unsigned int entry){ //defualt save in next entry so need to do lookup size to find it, overlad with entry number so as to overwrite in lookup table.
  //std::cout<<"bob save start="<<output.Btell()<<std::endl;  
  //std::cout<<"save m_entry="<<m_entry<<std::endl;

  m_update=true;
  
  std::cout<<"debug1 entry="<<entry<<std::endl;

  //  entry++;
  std::cout<<"m_lookup.size()="<<m_lookup.size()<<std::endl;
  if(entry>=m_lookup.size()){
    entry=m_lookup.size();
    m_lookup.resize(m_lookup.size()+1); 
  }
  std::cout<<"debug2 entry="<<entry<<std::endl;
  
  
  std::cout<<"debug3 entry="<<entry<<std::endl;
  
  std::cout<<"save start2="<<output.Btell()<<std::endl;
  std::cout<<"m_file_end="<<m_file_end<<std::endl;
  if(!output.Bseek(m_file_end,SEEK_SET)) return false;
  std::cout<<"save entry2="<<entry<<std::endl;
  m_lookup.at(entry)=output.Btell();
  std::cout<<"m_variables.size()="<<m_variables.size()<<std::endl;
  std::cout<<"m_lookup.size()="<<m_lookup.size()<<std::endl;
  std::cout<<"before saving m_variables="<<output.Btell()<<std::endl;
  if(!(output << m_variables)) return false;
  std::cout<<"after saving m_variables="<<output.Btell()<<std::endl;
  if(m_type_checking) if(!(output << m_type_info)) return false;
  m_file_end=output.Btell(); 
 
 
  return true;
}

bool BStore::GetHeader(){
  
  if(m_has_header){
    if(m_header!=0){
      delete m_header;
      m_header=0;
    }
    m_header= new BStore(false, false);
    output.Bseek(m_header_start, SEEK_SET);
    if(!(output >> m_header)) return false;
  }

  return true;
}

bool BStore::GetEntry(unsigned int entry_request){
  //std::cout<<"mode="<<output.m_mode<<std::endl;
  //  entry_request++;
  
  //std::cout<<"gettentry called="<<entry_request<<std::endl;

  //if(!entry_request) return false;  
  //std::cout<<"passed non zero check"<<std::endl;
  if((m_lookup.size()-1)<entry_request) return false;
  //if(!m_lookup.count(entry_request)) return false;
  //std::cout<<"mode="<<output.m_mode<<std::endl;
  //std::cout<<"passed get entry checks"<<std::endl;

  Delete();
  //std::cout<<"getting entry data: entry="<<entry_request<<", location is="<<m_lookup[entry_request]<<std::endl;  
  //std::cout<<"mode="<<output.m_mode<<std::endl;
  if(!output.Bseek(m_lookup[entry_request], SEEK_SET)) return false;
  //std::cout<<"mode="<<output.m_mode<<std::endl;  
  if(!(output >> m_variables))return false;
  //std::cout<<"mode="<<output.m_mode<<std::endl;
  if(m_type_checking)  if(!(output >>  m_type_info)) return false;
  //std::cout<<"mode="<<output.m_mode<<std::endl;
  return true;
  
  
}

// write header and lookup
bool BStore::WriteHeader(){

  m_header_start=output.Btell();
  if(m_has_header>0 && !(output << *m_header)) return false;
  m_file_end=output.Btell();

  return true;

}

bool BStore::WriteLookup(){

  m_lookup_start= output.Btell();
  m_lookup_size=m_lookup.size();
  m_previous_file_end=m_open_file_end;
  if(!(output << m_lookup)) return false;
  m_file_end=output.Btell();

  return true;
}

bool BStore::WriteFlags(){

  if(!(output << m_header_start)) return false;
  if(!(output << m_has_header)) return false; 
  if(!(output << m_lookup_start)) return false;
  if(!(output << m_lookup_size)) return false;
  if(!(output << m_type_checking)) return false;
  if(!(output << m_type)) return false;
  if(!(output << m_previous_file_end)) return false;
  return true;

}

bool BStore::Close(){
  
  if(!m_update) return output.Bclose(true);
  else{

    //std::cout<<"in close"<<std::endl;
    //std::cout<<"btell="<<output.Btell()<<std::endl;
    //std::cout<<"m_file_end"<<m_file_end<<std::endl;

    // write header and lookup
    if(!output.Bseek(m_file_end,SEEK_SET))return false;
    //    m_lookup[0]=output.Btell();                         old header location need to fix
   
    // m_header_start=output.Btell();
    // std::cout<<"btell="<<output.Btell()<<std::endl;
    //std::cout<<"saving header"<<std::endl;
    // if(m_has_header>0 && !(output << *m_header)) return false; 
    if(!WriteHeader()) return false;

    //std::cout<<"header saved"<<std::endl;
    //std::cout<<"btell="<<output.Btell()<<std::endl;
   
    // m_lookup_start= output.Btell();
    //m_lookup_size=m_lookup.size();
    //m_previous_file_end=m_open_file_end;
    //std::cout<<"saving lookup"<<std::endl;
    //if(!(output << m_lookup)) return false;
    //std::cout<<"saved lookup"<<std::endl;
    //std::cout<<"btell="<<output.Btell()<<std::endl;
    //m_file_end=output.Btell();
    //
    if(!WriteLookup()) return false;



    if(m_type!=uncompressed){

      if(!output.Bclose())return false;
      //std::cout<<"k1"<<std::endl;
      if(!output.Bopen(m_file_name,APPEND,UNCOMPRESSED))return false;
      //std::cout<<"k2 ="<<output.pfile<<std::endl;
    }
    

    //std::cout<<"k3 saving flag start pos="<<output.Btell()<<std::endl;
    if(!output.Bseek(0,SEEK_END))return false;
    //std::cout<<" k3 saving flag start pos2="<<output.Btell()<<std::endl;
    //std::cout<<"m_lookup_start="<< m_lookup_start<<std::endl;
    //std::cout<<"m_lookup_size="<< m_lookup_size<<std::endl;
    //std::cout<<"m_type_checking="<< m_type_checking<<std::endl;
    //std::cout<<"m_type="<< m_type<<std::endl;
    //std::cout<<"m_previous_file_end="<< m_previous_file_end<<std::endl;
   
    //write flags
    if(!WriteFlags()) return false;
    //if(!(output << m_header_start)) return false;
    //if(!(output << m_has_header)) return false; 
    //if(!(output << m_lookup_start)) return false;
    //if(!(output << m_lookup_size)) return false;
    //if(!(output << m_type_checking)) return false;
    //if(!(output << m_type)) return false;
    //if(!(output << m_previous_file_end)) return false;
    //std::cout<<"k4 "<<output.Btell()<<std::endl;
    if(!output.Bclose()) return false;
    //std::cout<<"k5"<<std::endl;
    Delete();    
    //std::cout<<"k6"<<std::endl;
    return true;
  }

}
//importing
  
bool BStore::Print(){ Print(false); return true;}

bool BStore::Print(bool values){
  
  for (std::map<std::string,BinaryStream>::iterator it=m_variables.begin(); it!=m_variables.end(); ++it){
    
    std::cout<< it->first << " => ";
    if(values) std::cout << it->second.buffer <<" :";
    std::cout<<" "<<m_type_info[it->first]<<std::endl;

  }
  
  for (std::map<std::string,PointerWrapperBase*>::iterator it=m_ptrs.begin(); it!=m_ptrs.end(); ++it){
    if(m_variables.count(it->first)==0){
      std::cout<< it->first << " => ";
      if(values) std::cout << it->second <<" :";
      std::cout<<" Pointer "<<std::endl;
    }
  }
  
  return true;
}

std::vector<std::string> BStore::GetKeys(){
    std::vector<std::string> keys;
    for (std::map<std::string,BinaryStream>::iterator it=m_variables.begin(); it!=m_variables.end(); ++it){
        keys.push_back(it->first);
    }
    for (std::map<std::string,PointerWrapperBase*>::iterator it=m_ptrs.begin(); it!=m_ptrs.end(); ++it){
        if(m_variables.count(it->first)==0) keys.push_back(it->first);
    }
    return keys;
}


void BStore::Delete(){ 
  
  m_variables.clear();
  m_type_info.clear();
 
  for (std::map<std::string,PointerWrapperBase*>::iterator it=m_ptrs.begin(); it!=m_ptrs.end(); ++it){

    delete it->second;
    it->second=0;

  }
  m_ptrs.clear();
  
  
}


void BStore::Remove(std::string key){

  for (std::map<std::string,BinaryStream>::iterator it=m_variables.begin(); it!=m_variables.end(); ++it){

    if(it->first==key){
      m_variables.erase(it);
      break;
    }
  }

    for (std::map<std::string,PointerWrapperBase*>::iterator it=m_ptrs.begin(); it!=m_ptrs.end(); ++it){

    if(it->first==key){
      delete it->second;
      it->second=0;
      m_ptrs.erase(it);
      break;
    }
  }
  

  if(m_type_checking){
    for (std::map<std::string,std::string>::iterator it=m_type_info.begin(); it!=m_type_info.end(); ++it){
      
      if(it->first==key){
	m_type_info.erase(it);
	break;
      }
    }
  }
  

}

void BStore::JsonParser(std::string input){
  
  int type=0;
  std::string key;
  std::string value;
  
  for(std::string::size_type i = 0; i < input.size(); ++i) {
    
    if(input[i]=='\"')type++;
    else if(type==1)key+=input[i];
    else if(type==3)value+=input[i];
    else if(type==4){
      type=0;
      m_variables[key] << value;
      key="";
      value="";
    }
  }
}


std::string BStore::Type(std::string key){

  if(m_type_info.count(key)>0){
    if(m_type_checking) return m_type_info[key];
    else return "?";
  }
  else return "Not in Store";

}

bool BStore::Has(std::string key){

  if(m_variables.count(key)>0) return true;
  else return false;

}

bool BStore::DeleteEntry(unsigned int entry_request){
 
  //Impliment this when you ahve changed it to a vector will be much easier

 /*
  entry_request++;

  if(!entry_request) return false;

  if(entry_request>=m_lookup.size()) return false;

  std::map<unsigned int, unsigned int>::iterator itbuff=m_lookup.end()

  for(std::map<unsigned int, unsigned int>::iterator it=m_lookup.end(); it!=m_lookup.begin(); it--){
    if(it->first==entry_request){
      itbuff=it;
      break;
    }   
    else {
      std::map<unsigned int, unsigned int>::iterator ittmp=it;
      it->second=itbuff->second;
      itbuff=ittmp;
    }
  }
  */

  return true;
  }


bool BStore::Rollback(){

  //ned to check if compressed or prepost to get location right sigh....

  Delete();
  m_lookup.clear();

  Initnew(m_file_name, m_type, m_has_header, m_type_checking, m_previous_file_end);    

  return true;
}


bool BStore::Serialise(BinaryStream &bs){ // do return properly
 
  /*   // uncomment here to start
  //writing 
  if(bs.m_write){
    if(not ram){
      Close();
      Initnew(reopen same file in ram);
    }     
    
    if(output.buffer.length()>0){ //meaning iv stored more than one event in ram
      if(m_update){
	if(!output.Bseek(m_file_end,SEEK_SET))return false;  
	if(!WriteHeader()) return false;
	if(!WriteLookup()) return false;
	if(!WriteFlags()) return false;
      }
      unsigned int size= output.buffer.length();
      unsigned int pos= bs.Btell();
      
      if(!(bs & pos)) return false;
      if(!(bs & size)) return false;
      if(!(bs & output)) return false;             
    }
    else{
      
      if(!(bs & m_variables)) return false;
      if typechecking    if(!(bs & m_variables)) return false;
      other variables, type etc
	
	}
  }

  
  else{ //reading
    unsigned int size=0;
    unsigned int pos=0;
    
    if(!(bs >> pos)) return false;
    if(!(bs >> size)) return false;
    
    
    if(bs.endpoint==RAM);  output.buffer=bs.buffer.substr(pos,pos+size);
    output.pfile=bs.pfile;
#ifdef ZLIB
    output.gzfile=bs.gzfile;
#endif
    output.m_mode=bs.m_mode;
    output.m_endpoint = bs.m_endpoint;
    if(!GetFlags(pos+size)) return false;
    if(!output.Bseek(m_lookup_start,SEEK_SET)) return false;
    if(!(output >> m_lookup)) return false;
    if(!GetHeader()) return false;  
    if(!GetEntry(0)) return false;
  }


 /*
   //std::cout<<"p1"<<std::endl;
    if(!(bs & m_variables)) return false;
    //std::cout<<"p2"<<std::endl;
    if(!(bs & m_type)) return false;
    //std::cout<<"p3"<<std::endl;
    if(!(bs & m_type_checking)) return false;
    //std::cout<<"p4"<<std::endl;
    if(m_type_checking){
    if(!(bs & m_type_info)) return false;
    }
    //std::cout<<"p5"<<std::endl;
    if(!(bs & m_header)) return false;
    //std::cout<<"p6"<<std::endl;
    if(!(bs & m_lookup)) return false;
    //std::cout<<"p7"<<std::endl;
    */

    return true;
    
  }




//////////////////////////////////////////////////////////////
/// entry 1 size
/// entry 1 
/// entry 2 size
/// entry 2 
/// entry 2 extra data for nesting 
/// entry 3 size
/// entry 3
/// ..
/// ..
/// Header / entry 0
/// lookup 0                                        :  lookup_start
/// lookup 1 
/// ..
/// ..
/// lookup_start                                    :  flags_start
/// lookup_size
/// type   //
/// previous_file_end
//////////////////////////////////////////////////  :  file_end


