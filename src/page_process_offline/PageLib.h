#ifndef _PAGELIB_H_
#define _PAGELIB_H_

#include "tinyxml2.h"
#include "../../include/simhash/include/simhash/Simhasher.hpp"
#include <string>
#include <vector>
#include <iostream>
using std::cout;
using std::endl;

using std::string;
using std::vector;
using namespace tinyxml2;
using namespace simhash;

struct RssItem
{
    string title;
    string link;
    string description;
    string content;
};

class PageLib
{
public:
    PageLib()
    : simhasher("../../include/simhash/dict/jieba.dict.utf8", "../../include/simhash/dict/hmm_model.utf8", "../../include/simhash/dict/idf.utf8", "../../include/simhash/dict/stop_words.utf8")
    {}

    ~PageLib() {}

    void Let_Me_See_See(const char* _dir);
    void dump(const string& fileName, int& cnt);

private:
    void processItem(XMLElement*);
    string getContent(const string&, const string&);
    uint64_t calc_simhash(const string&);
    bool isSimilar(uint64_t);

    vector<RssItem> _rss;
    vector<uint64_t> simhash_values;
    Simhasher simhasher;
};

#endif