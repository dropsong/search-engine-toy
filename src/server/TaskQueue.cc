#include "TaskQueue.h"

TaskQueue::TaskQueue(size_t queSize)
: _queSize(queSize)
, _que()
, _mutex()
, _notEmpty(_mutex)
, _notFull(_mutex)
, _flag(true)
{

}

TaskQueue::~TaskQueue()
{

}

bool TaskQueue::empty() const
{
    return 0 == _que.size();
}

bool TaskQueue::full() const
{
    return _que.size() == _queSize;
}

void TaskQueue::push(ElemType &&value)
{
    MutexLockGuard autoLock(_mutex);

    while(full())
    {
        _notFull.wait();//让生产者等待在条件变量上
    }

    _que.push(std::move(value));

    _notEmpty.notify();//唤醒消费者
}

ElemType TaskQueue::pop()
{
    MutexLockGuard autoLock(_mutex);

    while(_flag && empty())
    {
        _notEmpty.wait();//让消费者处于睡眠状态
    }

    if(_flag)
    {
        ElemType tmp = _que.front();
        _que.pop();

        _notFull.notify();//唤醒生产者
        return tmp;
    }
    else
    {
        return nullptr;
    }
}

void TaskQueue::wakeup()
{
    //唤醒所有等待在_notEmpty条件变量上的线程
    //也就是：消费者线程
    _flag = false;
    _notEmpty.notifyAll();
}

