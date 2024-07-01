// myRedis.h
#ifndef __MYREDIS_H__
#define __MYREDIS_H__

#include <hiredis/hiredis.h>
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::string;

class MyRedis{
public:
    MyRedis();
    ~MyRedis();
    bool connect(const string& host, int port);
    void set(string key, string value, int expireTime = 0);
    string get(string key);
    void del(string key);

private:
    redisContext* _pConnect;
    redisReply* _pReply;
};

#endif
