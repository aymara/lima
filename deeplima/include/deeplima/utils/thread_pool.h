// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INCLUDE_THREAD_POOL_H
#define DEEPLIMA_SRC_INCLUDE_THREAD_POOL_H

#include <iostream>
#include <assert.h>
#include <queue>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace  deeplima
{

template <class P>
class ThreadPool
{
public:

  ThreadPool(size_t num_threads = 0)
    : m_stop(false)
  {
    if (num_threads > 0)
    {
      init(num_threads);
    }
  }

  void init(size_t num_threads)
  {
    assert(num_threads > 0);
    assert(m_workers.size() == 0);

    m_workers.reserve(num_threads);
    for (size_t i = 0; i < num_threads; i++)
    {
      m_workers.emplace_back(std::thread(&ThreadPool::thread_fn, this, i));
    }
  }

  size_t get_num_threads() const
  {
    return m_workers.size();
  }

  virtual ~ThreadPool()
  {
    // std::cerr << "-> ~ThreadPool" << std::endl;
    stop();
    // std::cerr << "<- ~ThreadPool" << std::endl;
  }

  void stop()
  {
    m_stop = true;
    // push null jobs to ensure having enough joinable jobs
    for (size_t i = 0 ; i < m_workers.size(); i++)
    {
      push(nullptr);
    }

    for (auto& t : m_workers)
    {
      if (t.joinable())
      {
        t.join();
      }
    }

    while (!m_workers.empty())
    {
      if (!m_workers.back().joinable())
      {
        m_workers.pop_back();
      }
      else
      {
        throw std::runtime_error("All workers must be unjoinable (inactive threads) here.");
      }
    }
  }

  size_t running()
  {
    return m_workers.size();
  }

  inline void push(void* job)
  {
    std::lock_guard<std::mutex> l(m_mutex);
    m_jobs.push(job);
    m_cv.notify_one();
  }

protected:

  /** This will wait until a job is available and then @ref job parameter will
   * be set to this available which will be removed from the list.
   */
  inline bool wait_for_new_job(void** job)
  {
    std::unique_lock<std::mutex> l(m_mutex);
    m_cv.wait(l, [this](){ return !m_jobs.empty() || m_stop; });

    if (!m_jobs.empty())
    {
      *job = m_jobs.front();
      m_jobs.pop();

      return true;
    }

    return false;
  }

  inline void wait_for_any_job_notification(const std::function<bool()> fn)
  {
    std::unique_lock<std::mutex> l(m_mutex_notify);
    m_cv_notify.wait(l, [&fn](){ return fn(); });
  }

  void thread_fn(size_t worker_id)
  {
    void* job = nullptr;
    // loop to dispatch pushed jobs to the threads of this pool
    // wait_for_new_job is blocking until a job becomes available
    while (true)
    {
      // std::cerr << "thread_fn " << worker_id << " main loop" << std::endl;
      if (wait_for_new_job(&job))
      {
        // std::cerr << "wait_for_new_job is true" << std::endl;
        if (nullptr == job)
        {
          // we should get a null job only when stopping
          // std::cerr << "wait_for_new_job: we should get a null job only when stopping" << std::endl;
          break;
        }
        // std::cerr << "worker: running job " << (void*) job << std::endl;
        P::run_one_job(static_cast<P*>(this), worker_id, job);
        // std::cerr << "worker: completed job " << (void*) job << std::endl;
        m_cv_notify.notify_all();
        // std::cerr << "notify_all done" << std::endl;
      }
      else
      {
        break;
      }
    }

    if (!m_stop)
    {
      throw std::runtime_error("Worker finished but stop flag isn't set.");
    }
    // std::cerr << "thread_fn done" << std::endl;
  }

  std::vector<std::thread> m_workers;
  std::atomic<bool> m_stop;

  std::queue<void*> m_jobs;
  std::mutex m_mutex;
  std::condition_variable m_cv;

  std::mutex m_mutex_notify;
  std::condition_variable m_cv_notify;
};

} // namespace deeplima

#endif
