#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

EventLoop::EventLoop(Acceptor &acceptor)
: _epfd(createEpollFd())
, _evtfd(createEventFd())
, _isLooping(false)
, _acceptor(acceptor)
, _evtList(1024)
, _mutex()
{
    //将listenfd放在红黑树上进行监听
    addEpollReadFd(_acceptor.fd());
    //eventfd也需要进行监听
    addEpollReadFd(_evtfd);
}

EventLoop::~EventLoop() 
{
    close(_epfd);
    close(_evtfd);
}

void EventLoop::loop()
{
    _isLooping = true;
    while(_isLooping)
    {
        waitEpollFd();
    }
}

void EventLoop::unloop()
{
    _isLooping = false;
}

void EventLoop::setConnectionCallback(TcpConnectionCallback &&cb)
{
    _onConnectionCb = std::move(cb);
}

void EventLoop::setMessageCallback(TcpConnectionCallback &&cb)
{
    _onMessageCb = std::move(cb);
}

void EventLoop::setCloseCallback(TcpConnectionCallback &&cb)
{
    _onCloseCb =  std::move(cb);
}

void EventLoop::waitEpollFd()
{
    int nready;
    do
    {
        /* ::epoll_wait(_epfd, _evtList.data(), _evtList.size(), 5000); */
        nready = ::epoll_wait(_epfd, &*_evtList.begin(), _evtList.size(), 5000);
    }while(-1 == nready && errno == EINTR);
    
    if(-1 == nready)
    {
        perror("epoll_wait error\n");
        return;
    }
    else if(0 == nready)
    {
        cout << ">>epoll_wait timeout" << endl;
    }
    else
    {
        //考虑到扩容的问题，因为初始情况监听的文件描述符只写了1024
        if(nready == (int)_evtList.size())
        {
            _evtList.resize(2 * nready);
        }
        
        for(int idx = 0; idx < nready; ++idx)
        {
            int fd = _evtList[idx].data.fd;
            //如果fd是我们监听的listenfd
            if(fd == _acceptor.fd())
            {
                if(_evtList[idx].events & EPOLLIN)
                {
                    //处理新的连接
                    handleNewConnection();
                }
            }
            else if(fd == _evtfd)
            {
                if(_evtList[idx].events & EPOLLIN)
                {
                    handleRead();
                    doPengdingFunctors();//执行所有的任务
                }
            }
            else
            {
                //如果是老的连接,connfd
                if(_evtList[idx].events & EPOLLIN)
                {
                    //处理老的连接上发过来的数据
                    handleMessage(fd);
                }
            }
        }
    }
}

void EventLoop::handleNewConnection()
{
    int connfd = _acceptor.accept();
    if(connfd < 0)
    {
        perror("handleNewConnection\n");
        return;
    }
    //将新的文件描述符connfd放到红黑树进行监听
    addEpollReadFd(connfd);
    
    //如果connfd是正常的，那么就表明三次握手成功，表明连接就建立好了
    TcpConnectionPtr con(new TcpConnection(connfd, this));

    //注册三个回调函数
    con->setConnectionCallback(_onConnectionCb);//连接建立
    con->setMessageCallback(_onMessageCb);//消息到达
    con->setCloseCallback(_onCloseCb);//连接断开

    _conns.insert(std::make_pair(connfd, con));

    //满足连接建立的条件，所以执行该回调函数
    con->handleConnectionCallback();//处理连接建立
}

void EventLoop::handleMessage(int fd)
{
    auto it = _conns.find(fd);
    if(it != _conns.end())
    {
        bool flag = it->second->isClosed();
        //如果连接断开
        if(flag)
        {
            it->second->handleCloseCallback();//处理连接断开
            delEpollReadFd(fd);//将fd从红黑树上删除
            _conns.erase(it);//将connfd与连接这个键值对从map中删除
        }
        else
        {
            //如果连接没有断开
            it->second->handleMessageCallback();//处理消息的发送
        }
    }
    else
    {
        cout << "该连接不存在 " << endl;
        return;
    }
}

int EventLoop::createEpollFd()
{
    int fd = ::epoll_create1(0);
    if(fd < 0)
    {
        perror("epoll_create1\n");
        return fd;
    }

    return fd;
}

void EventLoop::addEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fd;

    int ret = ::epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &evt);
    if(ret < 0)
    {
        perror("epoll_ctl add\n");
        return;
    }
}

void EventLoop::delEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.data.fd = fd;

    int ret = ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &evt);
    if(ret < 0)
    {
        perror("epoll_ctl del\n");
        return;
    }
}

void EventLoop::wakeup()
{
    // 执行这个函数说明要开始线程通信了
    uint64_t one;
    ssize_t ret = write(_evtfd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("write\n");
        return;
    }
}

void EventLoop::runInLoop(Functor &&cb)
{
    // 把真正发送数据的函数再次传给 _pendings

    {
    // 若不加 {} 则锁的粒度太大了(锁的范围太大了，锁的代码函数多了)
        MutexLockGuard autoLock(_mutex);
        _pendings.push_back(std::move(cb));
    }

    //必须要执行唤醒操作
    wakeup();
    // 线程池唤醒 IO 线程
    // 执行这个函数说明要开始线程通信了
}

void EventLoop::handleRead()
{
    uint64_t one;
    ssize_t ret = read(_evtfd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("read\n");
        return;
    }
}

int  EventLoop::createEventFd()
{
    int fd = eventfd(10, 0);
    if(fd < 0)
    {
        perror("eventfd\n");
    }

    return fd;
}

void EventLoop::doPengdingFunctors()
{
    vector<Functor> tmp;//局部变量
    {
        MutexLockGuard autoLock(_mutex);
        tmp.swap(_pendings);
    }
    //对共享资源vector进行了读操作
    //在读操作的过程中有没有写的可能呢？那如果有，
    //就会发生读与写共存的现象，那么这种情况就
    //有可能发生意想不到bug
    for(auto &cb: tmp)
    {
        cb();
    }
}
