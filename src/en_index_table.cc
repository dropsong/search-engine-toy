#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <string>

using std::set;
using std::string;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::istringstream;

class MakeEnIndex
{
public:
    MakeEnIndex() = default;

    void readFile(const string& ifileName)
    {
        ifstream ifs(ifileName);
        if(!ifs.good()) {
            cerr << "file " << ifileName << "open failed." << endl;
            return;
        }

        string line;
        size_t line_no = 0;
        while(getline(ifs, line)) {
            ++ line_no;
            string word, unwanted_num;
            istringstream iss(line);
            while(iss >> word >> unwanted_num) {
                for(size_t i = 0; i < word.size(); ++i)
                {
                    alphabet_pos[word[i]-'a'].insert(line_no);
                }
            }
        }
        ifs.close();
    }

    void store(const string& ofileName)
    {
        ofstream ofs(ofileName);
        if(!ofs.good()) {
            cerr << "file " << ofileName << "open failed." << endl;
            return;
        }

        for(size_t i = 0; i < 26; ++i)
        {
            ofs << (char)('a'+i) << " ";
            auto it = alphabet_pos[i].cbegin();
            for(; it != alphabet_pos[i].cend(); ++it)
            {
                ofs << *it << " ";
            }
            ofs << endl;
        }
        ofs.close();
    }

private:
    set<int> alphabet_pos[26];
};

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        cerr << "args error" << endl;
        return 1;
    }
    string from_file(argv[1]);
    string to_file(argv[2]);

    MakeEnIndex mei;
    mei.readFile(from_file);
    mei.store(to_file);
    return 0;
}