/*
    Copyright 2021 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/

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
    std::cerr << "-> ~ThreadPool" << std::endl;
    stop();
    std::cerr << "<- ~ThreadPool" << std::endl;
  }

  void stop()
  {
    m_stop = true;
    for (auto& t : m_workers)
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
        throw std::runtime_error("All workers must be joiable here.");
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
    while (true)
    {
      if (wait_for_new_job(&job))
      {
        if (nullptr == job)
        {
          break;
        }
        //std::cerr << "worker: " << (void*) job << " started" << std::endl;
        P::run_one_job(static_cast<P*>(this), worker_id, job);
        //std::cerr << "worker: " << (void*) job << " completed" << std::endl;
        m_cv_notify.notify_all();
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
