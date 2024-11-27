#ifndef JOBDEQUE_H
#define JOBDEQUE_H

#include <deque>
#include <Job.h>
#include <mutex>

namespace ToolFramework{

  /**
   * \class JobDeque
   *
   * A class that is a deque of jobs that have been completed
   *
   * $Author: B.Richards $
   * $Date: 2024/06/08 1:17:00 $
   */
  
  class JobDeque{
    
  public:
    
    JobDeque(); ///< simple constructor
    ~JobDeque(); ///< simple destructor
    unsigned int size(); ///< return the number of jobs in the deque
    void push_back(Job* job); ///< add a job to the deque. @param job job to add
    Job* GetJob(std::string id); ///< function to pop a job off the queue. it returns the first job with the matching id @param id the id of the job to retrieve
    
  private:
    
    std::deque<Job*> m_jobs;
    std::mutex m_lock;
    
  };
  
}

#endif
