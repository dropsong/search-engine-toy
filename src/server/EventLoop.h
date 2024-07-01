#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "MutexLock.h"

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <functional>

using std::vector;
using std::map;
using std::shared_ptr;
using std::cout;
using std::endl;
using std::function;

class Acceptor;//类的前向声明
class TcpConnection;//类的前向声明


using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void(const TcpConnectionPtr &con)>;
using Functor = function<void()>;

class EventLoop
{
public:
    EventLoop(Acceptor &acceptor);
    ~EventLoop();
    //事件循环
    void loop();

    //事件不循环
    void unloop();

    //EventLoop作为中转，需要将三个回调函数进行注册，然后交给
    //TcpConnection,但是EventLoop不负责回调函数的执行，因为
    //EventLoop只有中转的作用，没有执行的能力，因为连接
    //肯定是与TcpConnection直接相关的
    void setConnectionCallback(TcpConnectionCallback &&cb);
    void setMessageCallback(TcpConnectionCallback &&cb);
    void setCloseCallback(TcpConnectionCallback &&cb);

private:
    //里面执行epoll_wait
    void waitEpollFd();

    //处理新的连接
    void handleNewConnection();

    //处理消息的收发
    void handleMessage(int fd);

    //创建epfd,封装了epoll_create
    int createEpollFd();

    //文件描述符放在红黑树上监听
    void addEpollReadFd(int fd);
    
    //将文件描述符从红黑树上删除
    void delEpollReadFd(int fd);

public:
    void wakeup();
    void runInLoop(Functor &&cb);

private:
    void handleRead();
    int createEventFd();
    void doPengdingFunctors();

private:
    int _epfd;//红黑树的根节点
    int _evtfd;//eventfd创建的文件描述符
    bool _isLooping;//循环是否进行的标志位
    Acceptor &_acceptor;//接收器的引用，为了调用accept函数
    vector<struct epoll_event> _evtList;//存放满足条件的事件
    map<int, TcpConnectionPtr> _conns;//存放文件描述符与TcpConnection的键值对

    TcpConnectionCallback _onConnectionCb;
    TcpConnectionCallback _onMessageCb;
    TcpConnectionCallback _onCloseCb;

    vector<Functor> _pendings;
    MutexLock _mutex;//vector是共享资源，需要互斥访问
};

#endif
