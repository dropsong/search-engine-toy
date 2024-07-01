#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "NonCopyable.h"
#include <pthread.h>

//前向声明
class MutexLock;

class Condition
: NonCopyable
{
public:
    Condition(MutexLock &mutex);
    ~Condition();
    void wait();
    void notify();
    void notifyAll();

private:
    MutexLock &_mutex;
    pthread_cond_t _cond;
};

#endif
