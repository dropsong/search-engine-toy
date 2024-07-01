#include "ThreadPool.h"
#include "Thread.h"
#include <unistd.h>

ThreadPool::ThreadPool(size_t threadNum, size_t queSize)
: _threadNum(threadNum)
, _queSize(queSize)
, _taskQue(_queSize)
, _isExit(false)
{
    //为了防止vector频繁扩容，所以可以提前预留空间
    _threads.reserve(_threadNum);
}

ThreadPool::~ThreadPool()
{
    //如果线程池退出标志位为1，就说明线程池
    //中的工作线程都已经退出来了，此时可以
    //什么都不做；但是如果线程池的标志位
    //为0，那么就表明子线程还没有完全退出
    if(!_isExit)
    {
        //让工作线程退出
        stop();
    }
}

//ctrl + v   j   shift + i 输入相应的关键字  esc
void ThreadPool::start()
{
    //将所有的工作线程存放在vector
    for(size_t idx = 0; idx != _threadNum; ++idx)
    {
        //unique_ptr不能复制或者赋值,具有对象语义
        //unique_ptr可以作为容器的元素，因为具有移动语义
        unique_ptr<Thread> up(new Thread(bind(&ThreadPool::threadFunc, this)));//注册
        _threads.push_back(std::move(up));//不能传左值
    }
    
    for(auto &th : _threads)
    {
        //让每一个工作线程运行起来
        th->start();
    }
}

void ThreadPool::stop()
{
    //只要任务队列中有数据  就不应该让工作线程退出来
    while(!_taskQue.empty())
    {
        //如果有数据，程序就应该卡在这里
        //什么都不写的时候，CPU是不是就没有任务，那就会空转
        sleep(1);
    }

    _isExit = true;//线程要退出，就置位标志位为true
    _taskQue.wakeup();//唤醒所有在睡觉的工作线程

    for(auto &th : _threads)
    {
        //让每一个工作线程退出
        th->join();
    }
}

void ThreadPool::addTask(Task &&taskcb)
{
    //指针在使用之前一定要判空，
    //否则会发生意想不到的问题
    if(taskcb)
    {
        _taskQue.push(std::move(taskcb));
    }
}

Task ThreadPool::getTask()
{
    return _taskQue.pop();
}

//线程池交给工作线程做的具体逻辑
void ThreadPool::threadFunc()
{
    //线程池中的工作线程会连续做事情，只要任务队列TaskQueue中
    //有任务，那么工作线程就会持续执行任务
    //
    //主要的问题在于任务process执行的快慢？
    //如果process执行的比较慢的话，那么_isExit是有足够时间设置
    //为true，这样就进不到while循环，那么不会调用getTask，也就
    //不会调用_taskQue.pop(),也就不会让_notEmoty.wait()执行，
    //那么程序就不会卡死；如果process执行的非常快的话，那么
    //_isExit就没有足够时间这是为true，就会进入while，而此时
    //又没有任务，所以getTask会调用_taskQue.pop(),会卡在
    //_notEmpty.wait(),程序就卡死了
    while(!_isExit)
    {
        //先获取任务
        Task taskcb = getTask();//此处调用了pop，wait
        if(taskcb)
        {
            //在执行具体的操作
            taskcb();//回调函数的执行
        }
    }
}

