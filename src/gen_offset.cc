#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;

struct tmp {
    int docid;
    size_t start;
    size_t end;
};

class makeOffSet
{
public:
    makeOffSet(): docnum(0) {}

    void Let_me_look_look(const string& fileName)
    {
        ifstream ifs(fileName);
        if(!ifs.good()) {
            cerr << "file " << fileName << "open failed." << endl;
            return;
        }

        string line;
        size_t pre = 0;
        while(getline(ifs, line))
        {
            if(line == "<doc>")
            {
                ++ docnum;
            }
            else if(line == "</doc>")
            {
                info.push_back({docnum, pre, ifs.tellg()});
                pre = ifs.tellg();
            }
        }
    }

    void dump(const string& fileName)
    {
        ofstream ofs(fileName);
        if(!ofs.good()) {
            cerr << "file " << fileName << "open failed." << endl;
            return;
        }

        for(auto i : info)
        {
            ofs << i.docid << " " << i.start << " " << i.end << endl;
        }
    }

private:
    int docnum;
    vector<tmp> info;
};

int main(int argc, char* argv[])
{
    if(argc != 3) {
        cout << "args error !" << endl
             << "args format:" << endl
             << "<output_file> <input_file>" << endl;
        return -1;
    }

    makeOffSet mos;
    mos.Let_me_look_look(argv[2]);
    mos.dump(argv[1]);

    return 0;
}