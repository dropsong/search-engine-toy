// MyRedis.cc
#include "myRedis.h"

MyRedis::MyRedis():_pConnect(nullptr), _pReply(nullptr){
    // cout << "MyRedis()" << endl;
}

MyRedis::~MyRedis(){
    // cout << "~MyRedis()" << endl;

    if(_pConnect){
        redisFree(_pConnect);
        _pConnect = nullptr;
    }

    if(_pReply){
        freeReplyObject(_pReply);
        _pReply = nullptr;
    }
}

bool MyRedis::connect(const string& host, int port){
    _pConnect = redisConnect(host.c_str(), port);
    if(_pConnect == nullptr){
        return false;
    }

    if(_pConnect != nullptr && _pConnect->err){
        std::cerr << "connect error : " << _pConnect->errstr << endl;
        return false;
    }

    return true;  // 连接成功
}

void MyRedis::set(string key, string value, int expireTime){
    if (expireTime > 0) {
        _pReply = (redisReply*) redisCommand(_pConnect, "SET %s %s EX %d", key.c_str(), value.c_str(), expireTime);
    } else {
        _pReply = (redisReply*) redisCommand(_pConnect, "SET %s %s", key.c_str(), value.c_str());
    }

    if(_pReply){
        freeReplyObject(_pReply);
        _pReply = nullptr;
    }
}

string MyRedis::get(string key){
    _pReply = (redisReply*) redisCommand(_pConnect, "GET %s", key.c_str());

    if(_pReply){
        if(_pReply->type == REDIS_REPLY_STRING){
            string str = _pReply->str;

            freeReplyObject(_pReply);
            _pReply = nullptr;

            return str;
        }
        else {
            return "";
        }
    }

    else {
        return "";
    }
}

void MyRedis::del(string key){
    _pReply = (redisReply*) redisCommand(_pConnect, "DEL %s", key.c_str());

    if(_pReply){
        freeReplyObject(_pReply);
        _pReply = nullptr;
    }
}
