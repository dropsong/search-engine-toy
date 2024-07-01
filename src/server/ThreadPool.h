#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include "TaskQueue.h"
#include <vector>
#include <memory>
#include <functional>

using std::vector;
using std::unique_ptr;
using std::bind;
using std::function;

class Thread;//类的前向声明

using Task = function<void()>;

class ThreadPool
{
public:
    ThreadPool(size_t threadNum, size_t queSize);
    ~ThreadPool();

    //线程池的开始与结束
    void start();
    void stop();

    //添加任务
    void addTask(Task &&taskcb);

private:
    //获取任务
    Task getTask();

    //线程池交给工作线程做的所有逻辑全部封装在此
    //线程池交给工作线程做的任务
    void threadFunc();

private:
    size_t _threadNum;//子线程的数目
    size_t _queSize;//任务队列的大小
    vector<unique_ptr<Thread>> _threads;//存放工作线程的容器
    TaskQueue _taskQue;//任务队列子对象，存放任务
    bool _isExit;//线程池退出的标志


};

#endif
