#include "Thread.h"
#include <stdio.h>

//要用移动语义，就一定要移动彻底
Thread::Thread(ThreadCallback &&cb)
: _thid(0)
, _isRunning(false)
, _cb(std::move(cb))//注册回调函数
{
}

Thread::~Thread()
{
    if(_isRunning)
    {
        //让子线程与主线程分离，主线程不管子线程的回收
        pthread_detach(_thid);
        _isRunning = false;
    }
}

//线程启动函数
//this指针指向的是pth，指向的是new MyThread
void Thread::start()
{
    //1、threadFunc是一个成员函数，所以需要消除this指针影响
    //2、pthread_create第四个参数不能设置为空，不然在threadFunc
    //中没有对象去调用run方法,所以第四个参数需要传递对象进来
    int ret = pthread_create(&_thid, nullptr, threadFunc, this);
    if(ret)
    {
        perror("pthread_create\n");
        return;
    }

    _isRunning = true;
}

void Thread::join()
{
    if(_isRunning)
    {
        //shift + k(2, 3)
        int ret = pthread_join(_thid, nullptr);
        if(ret)
        {
            perror("pthread_join\n");
            return;
        }

        _isRunning = false;
    }
}

//静态成员函数
void *Thread::threadFunc(void *arg)
{
    Thread *pth1 = static_cast<Thread *>(arg);
    if(pth1)
    {
        //为了防止threadFunc没有this指针，所以可以在pthread_create
        //中作为参数传进来
        pth1->_cb();//执行回调函数
    }

    //让子线程主动退出
    pthread_exit(nullptr);
}

