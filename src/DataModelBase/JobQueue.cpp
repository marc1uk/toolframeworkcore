#include <JobQueue.h>

using namespace ToolFramework;

JobQueue::JobQueue(){}

JobQueue::~JobQueue(){

  m_lock.lock();
  while(m_jobs.size()){
    delete m_jobs.front();
    m_jobs.front()=0;
    m_jobs.pop();
  }
 m_lock.unlock();

}

bool JobQueue::AddJob(Job* job){

  if(job!=0 && job->func!=0){
    job->m_complete=false;
    job->m_in_progress=false;
    job->m_failed=false;
    m_lock.lock();
    m_jobs.push(job);
    m_lock.unlock();
    return true;
  }
  return false;
  
}

Job* JobQueue::GetJob(){

  m_lock.lock();
  if(!m_jobs.size()){
    m_lock.unlock();
    return 0;
  } 
  Job* ret = m_jobs.front();
  m_jobs.front()=0;
  m_jobs.pop();
  m_lock.unlock();
  return ret;
}

bool JobQueue::pop(){

  m_lock.lock();
  if(m_jobs.size()){
    m_jobs.pop();
    m_lock.unlock();
    return true;
  }
  m_lock.unlock();
  return false;
}

unsigned int JobQueue::size(){

  m_lock.lock();
  unsigned int tmp= m_jobs.size();
  m_lock.unlock(); 
  return tmp; 

}
