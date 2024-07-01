#include "../include/cppjieba/Jieba.hpp"
#include <iostream>
#include <time.h>
#include <sstream>
#include <fstream>
#include <map>
#include <set>
#include <vector>

const char* const DICT_PATH = "../include/cppjieba/dict/jieba.dict.utf8";
const char* const HMM_PATH = "../include/cppjieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../include/cppjieba/dict/user.dict.utf8";
const char* const IDF_PATH = "../include/cppjieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "../include/cppjieba/dict/stop_words.utf8";

using std::endl;
using std::cout;
using std::cerr;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::string;
using std::map;
using std::set;
using std::vector;

// Helper function to trim whitespace from both ends of a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \n\r\t");
    size_t last = str.find_last_not_of(" \n\r\t");
    return (first == string::npos || last == string::npos) ? "" : str.substr(first, last - first + 1);
}

class zhDictionary {
public:
    zhDictionary()
    :_jieba(DICT_PATH,HMM_PATH,USER_DICT_PATH,IDF_PATH,STOP_WORD_PATH)
    {
        // load stop words.
        ifstream stop_ifs("../data/stop_words_list/stop_words_zh.txt");
        if(!stop_ifs.good()) {
            cerr << "stop_words file open failed." << endl;
            return;
        }
        string line;
        while(getline(stop_ifs, line))
        {
            line = trim(line);
            if(line == string()) continue;
            stopwords.insert(line);
        }
    }

    void readFile(const string& ifileName) {
        ifstream ifs(ifileName);
        if(!ifs.good()) {
            cerr << "file " << ifileName << "open failed." << endl;
            return;
        }

        string txt((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        _jieba.Cut(txt, words, true);

        for (const auto& word : words) {
            updmap(word);
        }

        ifs.close();
        words.clear(); // words 是 jieba 存结果的
    }

    void store(const string& ofileName) {
        ofstream ofs(ofileName);
        if(!ofs.good()) {
            cerr << "file " << ofileName << "open failed." << endl;
            return;
        }

        auto it = word_cnt.cbegin();
        for(; it != word_cnt.cend(); it++) {
            ofs << it->first << "  " << it->second << endl;
        }
        ofs.close();
    }

    void test()
    {
        cout << "stopwords.size() = " << stopwords.size() << endl;
        int cnt = 5;
        for(auto it = stopwords.begin(); it!=stopwords.end(); it++)
        {
            cout << *it << " with length " << it->length() << endl;
            cnt--;
            if(cnt<=0) break;
        }
        cout << "----------" << endl;
        cout << "stopwords.count(\"啊\") = " << stopwords.count("啊") << endl;
        cout << "stopwords.count(trim(\"啊\")) = " << stopwords.count(trim("a")) << endl;
        cout << "----------" << endl;
    }
    
private:
    map<string,unsigned long> word_cnt;
    set<string> stopwords;
    vector<string> words;
    cppjieba::Jieba _jieba;

    bool Contain1orA(const string& s)
    {
        for(int i = 0; i < s.size(); ++i)
        {
            if(s[i]>='0'&&s[i]<='9') return true;
            if(s[i]>='a'&&s[i]<='z') return true;
            if(s[i]>='A'&&s[i]<='Z') return true;
        }
        return false;
    }
        
    void updmap(const string& tobeInsert) {
        if(tobeInsert == string()) return;
        if(Contain1orA(tobeInsert)) return;
        if(stopwords.count(trim(tobeInsert))) return;
        word_cnt[tobeInsert] ++;
    }
};

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <output_file> <input_file1> <input_file2> ..." << endl;
        return 1;
    }

    zhDictionary myDic;
    // myDic.test();

    for(int i = 2; argv[i]!=0; ++i)
    {
        time_t beg = time(NULL);
        myDic.readFile(argv[i]);
        time_t end  = time(NULL);
        cout << "file" << i-1 << " process time : " << (end - beg) << "s" << endl;
    }
    myDic.store(argv[1]);
    cout << "done." << endl;

    return 0;
}