#ifndef LOG_H
#define LOG_H

#include <cstdio>
#include <iostream>
#include <string>
#include <pthread.h>
#include "../utils/block_queue.h"

using namespace std;

class Log
{
public:
    static Log* getInstance()
    {
        static Log instance;
        return &instance;
    }

    static void *flush_log_thread(void *args)
    {
        Log::getInstance()->async_write_log();
    }

    bool init(const char* file_name, int close_log, int log_buf_size = 8192, int split_lines = 5000000, int max_queue_size = 0);

    void write_log(int level, const char *format, ...);

    void flush(void);
private:
    Log();
    virtual ~Log();

    void* async_write_log();
    {
        string single_log;
        while (m_log_queue->pop(single_log))
        {
            m_mutex.lock();
            fputs(single_log.c_str(), m_fp);
            m_mutex.unlock();
        }
    }
private:
    char dir_name[128]; //路径
    char file_name[128];    //文件名
    int m_split_lines;  //最大行数
    int m_log_buf_size; //缓冲区大小
    long long m_count;  //行数
    int m_today;    //今天
    FILE* m_fp;     //打开的文件
    char* m_buf;    //缓冲区
    Block_Queue<string> *m_log_queue;   //阻塞队列
    bool m_is_async;    //是否同步
    locker m_mutex;
    int m_close_log;
};

#define LOG_DEBUG(format, ...) if(0 == m_close_log) { Log::getInstance()->write_log(0, format, ##__VA_ARGS__); Log::getInstance()->flush();}
#define LOG_INFO(format, ...) if (0 == m_close_log) { Log::getInstance()->write_log(1, format, ##__VA_ARGS__); Log::getInstance()->flush();}
#define LOG_WARN(format, ...) if (0 == m_close_log) { Log::getInstance()->write_log(2, format, ##__VA_ARGS__); Log::getInstance()->flush();}
#define LOG_ERROR(format, ...) if(0 == m_close_log) { Log::getInstance()->write_log(3, format, ##__VA_ARGS__); Log::getInstance()->flush();}
#endif