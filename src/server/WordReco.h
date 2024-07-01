#ifndef _WORDRECO_H_
#define _WORDRECO_H_

#include <string>
#include <set>
#include <queue>

using std::string;
using std::set;
using std::priority_queue;

struct Candidate
{
    string   _word;
    int      _dist;
    int      _freq;

    bool operator < (const Candidate& rhs) const 
    {
        if(_dist > rhs._dist) return true;
        if(_dist < rhs._dist) return false;
        if(_freq > rhs._freq) return false;
        if(_freq < rhs._freq) return true;
        // 其他情况下我们可以随便指定一个大小
        return true;
    }
};

class WordReco
{
public:
    WordReco();
    ~WordReco() = default;
    string handle(const string&);

private:
    size_t nBytesCode(const char);
    size_t length(const string&);
    void findInFile(const string&, const string&);
    void workInFreq(const string&, const string&);
    int triple_min(const int&, const int&, const int&);
    int editDistance(const string& lhs, const string& rhs);
    void furina(int, bool, string&);

    bool ab[26];
    set<int> enSet;
    set<int> zhSet;
    string ensubs, zhsubs;
    priority_queue<Candidate> CanRes_en;
    priority_queue<Candidate> CanRes_zh;
};

#endif
