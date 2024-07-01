#ifndef _SEARCHENGINESERVER_H_
#define _SEARCHENGINESERVER_H_

#include "TcpServer.h"
#include "TcpConnection.h"
#include "ThreadPool.h"
#include "WordReco.h"
#include "WebSearch.h"
#include "../../include/cppjieba/Jieba.hpp"
#include <iostream>
#include <sstream>

const char* const DICT_PATH = "../../include/cppjieba/dict/jieba.dict.utf8";
const char* const HMM_PATH = "../../include/cppjieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../../include/cppjieba/dict/user.dict.utf8";
const char* const IDF_PATH = "../../include/cppjieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "../../include/cppjieba/dict/stop_words.utf8";

using std::cout;
using std::endl;
using std::istringstream;

class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &con)
    : _msg(msg)
    , _con(con)
    {
    }

    ~MyTask() {}

    void process(const cppjieba::Jieba& jj)
    {
        // 所有业务逻辑的处理

        int flag = parseInfo(_msg);
        if(flag < 0) {
            _con->sendInLoop("msg format error !\n");
            return;
        }

        string processed_msg;
        if(flag == 1) {
            processed_msg = _wordReco.handle(_msg);
        } else if(flag == 2) {
            processed_msg = _webSearch.handle(_msg, jj);
        }

        if(processed_msg=="") processed_msg = "nothing found.";
        processed_msg += "\n";

        // 将_msg发送数据给客户端
        _con->sendInLoop(processed_msg);
    }

private:
    int parseInfo(string msg)
    {
        istringstream iss(msg);
        int num; string s;
        iss >> num >> s;
        if(num == 1) {
            _msg = s;
            return 1;
        } else if(num == 2) {
            _msg = s;
            return 2;
        } else {
            // msg format error !
            return -1;
        }
    }

    string _msg;
    TcpConnectionPtr _con;
    WordReco _wordReco;
    WebSearch _webSearch;
};

class SearchEngineServer
{
public:
    SearchEngineServer(size_t threadNum, size_t queSize,
               const string& ip, unsigned short port)
    : _pool(threadNum, queSize)
    , _server(ip, port)
    , _jieba(DICT_PATH,HMM_PATH,USER_DICT_PATH,IDF_PATH,STOP_WORD_PATH)
    {}

    ~SearchEngineServer() {}

    void start()
    {
        _pool.start();

        using namespace std::placeholders;
        _server.setAllCallback(std::bind(&SearchEngineServer::onConnection, this, _1)
                             , std::bind(&SearchEngineServer::onMessage, this, _1)
                             , std::bind(&SearchEngineServer::onClose, this, _1));
        _server.start();
    }

    void stop()
    {
        _pool.stop();
        _server.stop();
    }

    void onConnection(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has connected!" << endl;
    }

    void onMessage(const TcpConnectionPtr &con)
    {
        string msg = con->receive();
        cout << ">>recv msg from client: " << msg << endl;

        MyTask task(msg, con);
        _pool.addTask(std::bind(&MyTask::process, task, std::cref(_jieba)));
    }

    void onClose(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has closed!" << endl;
    }

private:
    ThreadPool _pool;
    TcpServer _server;
    cppjieba::Jieba _jieba;
};

#endif