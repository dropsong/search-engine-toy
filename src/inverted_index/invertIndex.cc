#include "invertIndex.h"
#include <iostream>
#include <boost/regex.hpp>
#include <cmath>
#include <iomanip>

using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::ofstream;
using std::istringstream;

const char* const DICT_PATH = "../../include/cppjieba/dict/jieba.dict.utf8";
const char* const HMM_PATH = "../../include/cppjieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../../include/cppjieba/dict/user.dict.utf8";
const char* const IDF_PATH = "../../include/cppjieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "../../include/cppjieba/dict/stop_words.utf8";

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \n\r\t");
    size_t last = str.find_last_not_of(" \n\r\t");
    return (first == string::npos || last == string::npos) ? "" : str.substr(first, last - first + 1);
}

inline double log2base(double num)
{
    return std::log(num) / std::log(2.0);
}

InvertIndex::InvertIndex()
:_jieba(DICT_PATH,HMM_PATH,USER_DICT_PATH,IDF_PATH,STOP_WORD_PATH)
{}

InvertIndex::~InvertIndex()
{}

void InvertIndex::readFile(const string& offsetF, const string& webF)
{
    ifstream ifsO(offsetF);
    if(!ifsO.good()) {
        cerr << "file " << offsetF << "open failed." << endl;
        return;
    }
    ifstream ifsW(webF);
    if(!ifsW.good()) {
        cerr << "file " << webF << "open failed." << endl;
        return;
    }
    
    string line = "";
    int s, e, docid;
    while(getline(ifsO, line))
    {
        istringstream iss(line);
        iss >> docid >> s >> e;

        ifsW.seekg(s);
        string line2 = "", mydoc = "";
        while(getline(ifsW, line2))
        {
            mydoc += line2;
            if(ifsW.tellg() == e) break;
        }
        mydoc = getContent(mydoc);
        vector<string> words; // jieba use
        _jieba.Cut(mydoc, words, true);
        for(int i = 0; i != words.size(); i++)
        {
            // 计算某个词在某篇文章中出现的次数，即 TF
            InvertIndexTable[trim(words[i])][docid] += 1.0;
        }
    }

    ifsO.close();
    ifsW.close();

    for(auto& idx : InvertIndexTable)
    {
        double DF = idx.second.size();
        double IDF = log2base(static_cast<double>(docid)/(DF+1.0));
        for(auto& it : idx.second)
        {
            it.second *= IDF;
            calc_use[it.first] += (it.second*it.second);
        }
    }

    for(auto& idx : InvertIndexTable)
    {
        for(auto& it : idx.second)
        {
            InvertIndexTable[idx.first][it.first] 
                            /= sqrt(calc_use[it.first]);
        }
    }
    // TF-IDF algorithm
}

void InvertIndex::dump(const string& fileName)
{
    ofstream ofs(fileName);
    if(!ofs.good()) {
        cerr << "file " << fileName << "open failed." << endl;
        return;
    }

    ofs << std::fixed << std::setprecision(7);

    for(auto& idx : InvertIndexTable)
    {
        ofs << trim(idx.first) << " ";
        for(auto& it : idx.second)
        {
            ofs << it.first << " " << it.second << " ";
        }
        ofs << endl;
    }

    ofs.close();
}

string InvertIndex::getContent(const string& text)
{
    boost::regex re_description("<description>([\\s\\S]*?)</description>");  
    boost::regex re_content("<content>([\\s\\S]*?)</content>");  
      
    boost::smatch match, match2;  
    string description, content;  
  
    if (boost::regex_search(text, match, re_description)) {  
        if (match.size() > 1) {  
            description = match[1].str();  
            // cout << "Description 内容: " << description << endl;
        } else {  
            description = "";  
        }
    } else {  
        // cout << "没有匹配到 <description> 内容。" << endl;  
    }  
  
    if (boost::regex_search(text, match2, re_content)) {  
        if (match2.size() > 1) {  
            content = match2[1].str();  
            // cout << "Content 内容: " << content << endl;
        } else {  
            content = "";  
        }  
    } else {  
        // cout << "没有匹配到 <content> 内容。" << endl;  
    }

    if(content == "") return description;
    return content;
}

int main(int argc, char* argv[])
{
    if(argc != 4) {
        cout << "args error ! " << endl
             << "args format: " << endl
             << "<output_file> <offset_file> <webs_dat_file>" << endl;
        return -1;
    }

    InvertIndex iid;
    iid.readFile(argv[2], argv[3]);
    iid.dump(argv[1]);

    return 0;
}