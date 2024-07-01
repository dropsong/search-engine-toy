#include "WebSearch.h"
#include "useRedis.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

using std::ifstream;
using std::istringstream;

const string InvertIndexfile = "../../data/web_archive/invertIndexTable.dat";
const string offSetFile = "../../data/web_archive/offset.dat";
const string webFile = "../../data/web_archive/webs.dat";

string WebSearch::handle(const string& s, const cppjieba::Jieba& jj)
{
    string tmp = findInCache(s, "2");
    if(tmp != "")
    {
        tmp += "\n[this result is from cache]";
        return tmp;
    }

    jj.Cut(s, words, true);
    getBase();
    findInInvertIndex();
    bool myflag = getID(); // 网页搜索的结果，以 docid 排序
    if(!myflag) return "";

#if 0
    string testANS;
    for(auto& i : qwqVec)
    {
        testANS += std::to_string(i.docID) + "\n";
    }
    return testANS;
#endif

    findInWebs();
    // 搜索结果已填充了标题、链接、摘要，并按余弦相似度排序

    string ANS;
    for(auto& i : qwqVec)
    {
        ANS += ">> - - - - - - - - - - - - - - - - -\n";
        ANS += "title: " + i.title + "\n";
        ANS += "link:  " + i.link + "\n";
        ANS += "abstract:\n" + i.abstract + "\n";
        ANS += "- - - - - - - - - - - - - - - - - <<\n\n";
    }

    putInCache(s, ANS, "2");
    return ANS;
}

void WebSearch::getBase()
{
    for(int i = 0; i < words.size(); i++)
    {
        _base[words[i]] += 1.0;
    }
    double sigmaWi = 0.0;
    for(auto& i : _base)
    {
        sigmaWi += (i.second) * (i.second);
    }
    for(auto& i : _base)
    {
        i.second = i.second / sqrt(sigmaWi);
    }
}

void WebSearch::findInInvertIndex()
{
    ifstream ifs(InvertIndexfile);
    if(!ifs.good()) {
        cerr << "file " << InvertIndexfile << " open failed." << endl;
        return;
    }

    string line;
    while(getline(ifs, line))
    {
        string word;
        istringstream iss(line);
        iss >> word;
        if(!_base.count(word)) continue;
        int docid; double w;
        set<int> tmp;
        while(iss >> docid >> w)
        {
            tmp.insert(docid);
            intstr2W[{docid, word}] = w;
        }
        vs.push_back(tmp);
    }

    ifs.close();
}

bool WebSearch::getID()
{
    if(vs.empty()) return false;
    set<int> Res = vs[0];
    for(int i = 1; i < vs.size(); ++i)
    {
        set<int> tmp;
        std::set_intersection(Res.begin(), Res.end(),
                              vs[i].begin(), vs[i].end(),
                              std::inserter(tmp, tmp.begin()));
        Res = std::move(tmp);
    }
    // Res 里面保存了包含所有搜索词的文章的 docid
    if(Res.empty()) return false;

    for(auto& docid : Res)
    {
        // 对于每一篇文章，计算它与 base 的余弦值
        double sum = 0.0, sum2 = 0.0, sum3 = 0.0;
        for(auto& i : _base)
        {
            // sum 里保存了两向量的点积
            sum += intstr2W[{docid, i.first}] * i.second;
            sum2 += intstr2W[{docid, i.first}] * intstr2W[{docid, i.first}];
            sum3 += i.second * i.second;
        }
        sum2 = sqrt(sum2); // 该文章对应向量的模长
        sum3 = sqrt(sum3); // base 向量的模长
        double angle = sum / (sum2 * sum3); // 余弦相似度

        qwqVec.push_back((node){docid, angle});
    }
    return true;
}

void WebSearch::findInWebs()
{
    ifstream ifs(offSetFile);
    ifstream ifs2(webFile);
    if(!ifs.good()) {
        cerr << "file " << offSetFile << " open failed." << endl;
    }
    if(!ifs2.good()) {
        cerr << "file " << webFile << " open failed." << endl;
    }

    auto it = qwqVec.begin();
    string line;
    int s, e, docid;
    while(getline(ifs, line))
    {
        istringstream iss(line);
        iss >> docid >> s >> e;
        if(docid == it->docID) {

            ifs2.seekg(s); // webFile
            string line2, Doc;
            while(getline(ifs2, line2))
            {
                // std::cout << line2 << endl;
                Doc += line2;
                if(ifs2.tellg() == e) break;
            }

            string _titl = getContent(Doc, (boost::regex)"<title>([\\s\\S]*?)</title>");
            string _link = getContent(Doc, (boost::regex)"<link>([\\s\\S]*?)</link>");
            string _desc = getContent(Doc, (boost::regex)"<description>([\\s\\S]*?)</description>");
            string _cont = getContent(Doc, (boost::regex)"<content>([\\s\\S]*?)</content>");
            string _abst = choooose(_desc, _cont, _base.begin()->first);

            it->title = _titl;
            it->link = _link;
            it->abstract = _abst;

            if(++it == qwqVec.end()) break;
        }
    }

    sort(qwqVec.begin(), qwqVec.end());

    ifs.close();
    ifs2.close();
}

string WebSearch::getContent(const string& text, boost::regex ruuuule)
{
    boost::smatch match;
    string wanted;
  
    if (boost::regex_search(text, match, ruuuule) && match.size() > 1)
        wanted = match[1].str();  
    else wanted = "";  

    return wanted;
}

string WebSearch::choooose(const string& lhs, const string& rhs, const string& sw)
{
    string Sigewinne = rhs == "" ? lhs : rhs;
    if(Sigewinne =="")
    {
        // 实际上不应该走到这里
        // 若 xml 满足这样的条件，清理数据时就会报错
        return "this article has no content";
    }
    return findSentenceWithKeyword(Sigewinne, sw);
}

string WebSearch::findSentenceWithKeyword(const string& content, const string& keyword)
{
    string regexPattern = "(.{0,30})(" + keyword + ")(.{0,30})";
    boost::regex pattern(regexPattern);

    boost::smatch match;
    if (boost::regex_search(content, match, pattern)) {
        return "..." + match.str(1) + match.str(2) + match.str(3) + "...";
    }
    return "";
}