#include <JobDeque.h>

using namespace ToolFramework;

JobDeque::JobDeque(){}

JobDeque::~JobDeque(){
  
  m_lock.lock();
  while(m_jobs.size()){
    delete m_jobs.front();
    m_jobs.front()=0;
    m_jobs.pop_front();
  }
  m_lock.unlock();
  
}

unsigned int JobDeque::size(){
  m_lock.lock();
  unsigned int tmp= m_jobs.size();
  m_lock.unlock();
  return tmp;

}

void JobDeque::push_back(Job* job){
  m_lock.lock();
  m_jobs.push_back(job);
  m_lock.unlock();
}

Job* JobDeque::GetJob(std::string id){
  Job* job = 0;
  
  m_lock.lock();
  for(std::deque<Job*>::iterator it=m_jobs.begin(); it!=m_jobs.end(); it++){
    if((*it)->m_id == id){
      job = (*it);
      m_jobs.erase(it);
      break;
    }
    
  }
  
  m_lock.unlock();
  return job;
  
}
