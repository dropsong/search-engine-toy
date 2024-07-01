#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "Socket.h"
#include "SocketIO.h"
#include "InetAddress.h"

#include <memory>
#include <functional>

using std::shared_ptr;
using std::function;

class TcpConnection;//类的前向声明
class EventLoop;//类的前向声明

using TcpConnectionPtr = shared_ptr<TcpConnection>;

class TcpConnection
: public std::enable_shared_from_this<TcpConnection>
{
    using TcpConnectionCallback = function<void(const TcpConnectionPtr &con)>;
public:
    explicit TcpConnection(int fd, EventLoop *loop);
    ~TcpConnection();
    void send(const string &msg);
    void sendInLoop(const string &msg);
    string receive();
    //为了方便调试的函数
    string toString();
    bool isClosed() const;

    //三个回调函数的注册
    void setConnectionCallback(const TcpConnectionCallback &cb);//连接建立
    void setMessageCallback(const TcpConnectionCallback &cb);//消息到达
    void setCloseCallback(const TcpConnectionCallback &cb);//连接断开

    //三个回调函数的执行
    void handleConnectionCallback();//处理连接建立
    void handleMessageCallback();//处理消息的发送
    void handleCloseCallback();//处理连接断开

private:
    //获取本端地址与对端地址
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();

private:
    EventLoop *_loop;
    SocketIO _sockIO;

    //为了调试而加入的函数
    Socket _sock;
    InetAddress _localAddr;
    InetAddress _peerAddr;

    TcpConnectionCallback _onConnectionCb;
    TcpConnectionCallback _onMessageCb;
    TcpConnectionCallback _onCloseCb;
};

#endif
