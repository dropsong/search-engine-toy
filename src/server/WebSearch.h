#ifndef _WEBSEARCH_H_
#define _WEBSEARCH_H_

#include "../../include/cppjieba/Jieba.hpp"
#include <vector>
#include <map>
#include <set>
#include <string>
#include <boost/regex.hpp>

using std::vector;
using std::map;
using std::set;
using std::pair;
using std::string;

struct node {
    int docID;
    double cosSimi;
    string title;
    string link;
    string abstract;
    bool operator < (const node& rhs) const
    {
        return cosSimi < rhs.cosSimi;
    }
};

class WebSearch
{
public:
    string handle(const string&, const cppjieba::Jieba&);

private:
    void getBase();
    void findInInvertIndex();
    bool getID();
    void findInWebs();
    string getContent(const string&, boost::regex);
    string choooose(const string&, const string&, const string&);
    string findSentenceWithKeyword(const string&, const string&);

    vector<string> words; // jieba use
    map<string, double> _base; // string 是待查询字符串的分词（不重复）
    vector<set<int>> vs;  // 每个分词对应的 docid 集合
    map<pair<int, string>, double> intstr2W; // 某个docid里某个分词的TF-IDF权重
    vector<node> qwqVec; // 保存网页搜索的结果的 docid
};

#endif