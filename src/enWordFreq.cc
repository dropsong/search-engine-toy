#include <iostream>
#include <time.h>
#include <sstream>
#include <fstream>
#include <map>
#include <set>

using std::endl;
using std::cout;
using std::cerr;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::string;
using std::map;
using std::set;

// Helper function to trim whitespace from both ends of a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \n\r\t");
    size_t last = str.find_last_not_of(" \n\r\t");
    return (first == string::npos || last == string::npos) ? "" : str.substr(first, last - first + 1);
}

class Dictionary {
public:
    Dictionary()
    {
        // load stop words.
        ifstream stop_ifs("../data/stop_words_list/stop_words_eng.txt");
        if(!stop_ifs.good()) {
            cerr << "stop_words file open failed." << endl;
            return;
        }
        string line;
        while(getline(stop_ifs, line))
        {
            if(line == string()) continue;
            line = trim(line);
            stopwords.insert(line);
        }
    }

    void readFile(const string& ifileName) {
        ifstream ifs(ifileName);
        if(!ifs.good()) {
            cerr << "file " << ifileName << "open failed." << endl;
            return;
        }
        
        string line;
        while(getline(ifs, line)) {
            string word;
            istringstream iss(line);
            while(iss >> word) {
                string new_word = process_word(word);
                updmap(new_word);
            }
        }

        ifs.close();
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
        cout << "stopwords.count(\"a\") = " << stopwords.count("a") << endl;
        cout << "stopwords.count(trim(\"a\")) = " << stopwords.count(trim("a")) << endl;
        cout << "----------" << endl;
        
    }
    
private:
    map<string,unsigned long> word_cnt;
    set<string> stopwords;

    string process_word(const string& word) {
        string cleaned_word;
        for(char ch : word)
        {
            if(isalpha(ch))
                cleaned_word += tolower(ch);
        }
        return cleaned_word;
    }

    void updmap(const string& tobeInsert) {
        if(tobeInsert == string()) return;
        // 如果是 stopwords，我们也直接丢弃
        if(stopwords.count(tobeInsert))
        {
            // cout << "Stopword filtered: " << tobeInsert << endl;
            return;
        }
        // cout << "Word added: " << tobeInsert << endl; // 调试输出
        word_cnt[tobeInsert] ++;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];

    Dictionary myDic;

    // myDic.test();

    time_t beg = time(NULL);
    myDic.readFile(inputFile);
    myDic.store(outputFile);
    time_t end  = time(NULL);
    cout << "time : " << (end - beg) << "s" << endl;
    return 0;
}