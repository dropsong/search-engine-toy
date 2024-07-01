#include "WordReco.h"
#include "useRedis.h"
#include <cstring>
#include <fstream>
#include <sstream>

using std::ifstream;
using std::istringstream;

const string en_index_table = "../../data/wordReco_index_table/en/en_index_table.dat";
const string zh_index_table = "../../data/wordReco_index_table/zh/zh_index_table.dat";
const string en_wordFreq = "../../data/wordReco_wordFreq/en/oldSet.dat";
const string zh_wordFreq = "../../data/wordReco_wordFreq/zh/oldSet.dat";

WordReco::WordReco()
{
    memset(ab, false, sizeof(ab));
}

string WordReco::handle(const string& s)
{
    string tmp = findInCache(s, "1");
    if(tmp != "")
    {
        tmp += " [cache]";
        return tmp;
    }

    size_t len = length(s);
    for(size_t dist_i = 1, s_idx = 0; dist_i <= len; ++dist_i, ++s_idx)
    {
        size_t nBytes = nBytesCode(s[s_idx]);
        string subs = s.substr(s_idx, nBytes);
        s_idx += (nBytes - 1);
        if(nBytes == 1) {
            // a  b  c  d 
            ensubs += subs;
            if(ab[subs[0]-'a']) continue;
            ab[subs[0]-'a'] = true;
            findInFile(subs, en_index_table);
        } else {
            // 清  明  重  阳
            zhsubs += subs;
            findInFile(subs, zh_index_table);
        }
    }

    if(ensubs != "") workInFreq(en_wordFreq, ensubs);
    if(zhsubs != "") workInFreq(zh_wordFreq, zhsubs);

    string ans;
    if(ensubs != "" && zhsubs == "")
    {
        furina(5, true, ans);
    }
    else if(ensubs == "" && zhsubs != "")
    {
        furina(5, false, ans);
    }
    else if(ensubs != "" && zhsubs != "")
    {
        furina(3, true, ans);
        furina(3, false, ans);
    }

    putInCache(s, ans, "1");
    return ans;
}

size_t WordReco::nBytesCode(const char ch)
{
    if(ch & (1 << 7))
    {
        int nBytes = 1;
        for(int idx = 0; idx != 6; ++idx)
        {
            if(ch & (1 << (6 - idx)))
            {
                ++nBytes;	
            }
            else
                break;
        }
        return nBytes;
    }
    return 1;
}

size_t WordReco::length(const string& str)
{
    std::size_t ilen = 0;
    for(std::size_t idx = 0; idx != str.size(); ++idx)
    {
        int nBytes = nBytesCode(str[idx]);
        // 如果这个字符不止一个字节，就跳到它的最后一个字节
        idx += (nBytes - 1);
        ++ilen;
    }
    return ilen;
}

// 将词频库中含有 w 的行的行号存在 set 中
void WordReco::findInFile(const string& w, const string& fileName)
{
    ifstream ifs(fileName);
    if(!ifs.good()) {
        cerr << "file " << fileName << "open failed." << endl;
        return;
    }

    string line;
    while(getline(ifs, line))
    {
        string word;
        istringstream iss(line);
        iss >> word;
        if(word != w) continue;
        int num;
        while(iss >> num)
        {
            if(fileName == en_index_table)
                enSet.insert(num);
            else if(fileName == zh_index_table)
                zhSet.insert(num);
        }
        break; // 索引唯一，索引表后面不会再匹配到了
    }
    ifs.close();
}

void WordReco::workInFreq(const string& fileName, const string& ss)
{
    bool flag = fileName == en_wordFreq ? true : false ;
    ifstream ifs(fileName);
    if(!ifs.good()) {
        cerr << "file " << fileName << "open failed." << endl;
        return;
    }

    string line;
    int line_no = 0;
    auto it = enSet.cbegin();
    if(!flag) it = zhSet.cbegin();
    while(getline(ifs, line))
    {
        line_no ++;
        if(line_no != *it) continue;
        string word;  int num;
        istringstream iss(line);
        iss >> word >> num;
        int dist = editDistance(ss, word);
        if(dist <= 3)
        {
            if(flag)
                CanRes_en.push({word, dist, num});
            else 
                CanRes_zh.push({word, dist, num});
        }
        
        ++ it;
        if(it == enSet.cend()) break;
        if(it == zhSet.cend()) break;
    }
    ifs.close();
}

int WordReco::triple_min(const int &a, const int &b, const int &c)
{
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

int WordReco::editDistance(const string& lhs, const string& rhs)
{
    //计算最小编辑距离-包括处理中英文

    size_t lhs_len = length(lhs);
    size_t rhs_len = length(rhs);
    int editDist[lhs_len + 1][rhs_len + 1];

    for(size_t idx = 0; idx <= lhs_len; ++idx)
    {
        editDist[idx][0] = idx;
    }

    for(size_t idx = 0; idx <= rhs_len; ++idx)
    {
        editDist[0][idx] = idx;
    }
    
    std::string sublhs, subrhs;
    for(std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i, ++lhs_idx)
    {
        // start string
        size_t nBytes = nBytesCode(lhs[lhs_idx]);
        sublhs = lhs.substr(lhs_idx, nBytes);
        lhs_idx += (nBytes - 1);

        for(std::size_t dist_j = 1, rhs_idx = 0; dist_j <= rhs_len; ++dist_j, ++rhs_idx)
        {
            // target string
            nBytes = nBytesCode(rhs[rhs_idx]);
            subrhs = rhs.substr(rhs_idx, nBytes);
            rhs_idx += (nBytes - 1);

            if(sublhs == subrhs)
            {
                editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j - 1];
            }
            else
            {
                editDist[dist_i][dist_j] = triple_min(
                    editDist[dist_i][dist_j - 1] + 1,
                    editDist[dist_i - 1][dist_j] + 1,
                    editDist[dist_i - 1][dist_j - 1] + 1);
            }
        }
    }
    return editDist[lhs_len][rhs_len];
}

void WordReco::furina(int maxN, bool flag, string& ans)
{
    int num = 0;
    while( (!CanRes_en.empty()&&flag) || (!CanRes_zh.empty()&&!flag) )
    {
        Candidate node;
        if(flag) {
            node = CanRes_en.top();
            CanRes_en.pop();
        } else {
            node = CanRes_zh.top();
            CanRes_zh.pop();
        }
        ans += node._word + " ";
        if(++num >= maxN) break;
    }
}