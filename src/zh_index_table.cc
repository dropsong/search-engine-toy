#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <string>

using std::set;
using std::map;
using std::string;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::istringstream;

class MakeZhIndex
{
public:
    MakeZhIndex() = default;

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
                    int nBytes = nBytesCode(word[i]);
                    string zh_zi = word.substr(i, nBytes);
                    zi2set[zh_zi].insert(line_no);
                    i += (nBytes - 1);
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

        auto it = zi2set.cbegin();
        for(; it != zi2set.cend(); ++it)
        {
            ofs << it->first << "  ";
            auto it2 = it->second.cbegin();
            for(; it2 != it->second.cend(); ++it2)
            {
                ofs << *it2 << " ";
            }
            ofs << endl;
        }

        ofs.close();
    }

private:
    map<string, set<int>> zi2set;

    size_t nBytesCode(const char ch)
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

    MakeZhIndex mzi;
    mzi.readFile(from_file);
    mzi.store(to_file);
    return 0;
}