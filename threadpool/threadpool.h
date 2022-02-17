#ifndef PROCESSPOOL_H
#define PROCESSPOOL_H

#include <cstdio>
#include <pthread.h>
#include <exception>
#include <list>
#include "../lock/lock.h"

template <typename T>
class pthreadpool
{
public:
    pthreadpool(int thread_number = 8, int request_max_number = 1000);
    ~pthreadpool();
    
    bool append(T* request);

    static void* work(void* arg);

    void run();
private:
    int m_thread_number；
    int m_request_max_number;
    pthread_t* m_threads;
    std::list<T*> m_workqueue;
    locker m_queuelocker;
    sem m_queuestat;
    bool m_stop;
};

template <typename T>
pthreadpool::pthreadpool(int thread_number = 8, int request_max_number = 1000):m_thread_number(thread_number), m_stop(false), m_threads(NULL)
{
    if (thread_number <= 0 || request_max_number <= 0)
    {
        throw std::exception();
    }
    m_threads = new pthread_t[m_thread_number];
    if (m_threads == NULL)
    {
        throw std::exception();
    }
    for (int i = 0; i < m_thread_number; ++i)
    {
        if (pthread_create(m_threads + i, NULL, work, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
        if (pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}

template <typename T>
pthreadpool::~pthreadpool()
{
    delete[] m_threads;
    m_stop = true;
}

template <typename T>
bool pthreadpool::append(T* request)
{
    m_queuelocker.lock();
    if (m_workqueue.size() <= m_request_max_number)
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template <typename T>
void* pthreadpool::work(void* arg)
{
    threadpool* pool = (threadpool*)arg;
    pool->run();
    return pool;
}

template <typename T>
void pthreadpool::run()
{
    while (!m_stop)
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if (m_workqueue.size <= 0)
        {
            m_queuelock.unlock();
            continue;
        }
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelock.unlock();
        if ( !request )
        {
            continue;
        }
        request->process();
    }
}
#endif
