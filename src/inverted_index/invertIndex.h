#ifndef _INVERTINDEX_H_
#define _INVERTINDEX_H_

#include "../../include/cppjieba/Jieba.hpp"
#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::unordered_map;
using std::vector;
using std::pair;

class InvertIndex
{
public:
    InvertIndex();
    ~InvertIndex();
    void readFile(const string&, const string&);
    void dump(const string&);

private:
    string getContent(const string&);

    unordered_map<string, unordered_map<int, double>> InvertIndexTable;
    unordered_map<int, double> calc_use;
    cppjieba::Jieba _jieba;
};

#endif