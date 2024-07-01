#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;

int main()
{
    ifstream ifs("offset.dat");
    ifstream ifs2("webs.dat");
    string line;
    int s, e, docid;
    while(getline(ifs, line))
    {
        istringstream iss(line);
        iss >> docid >> s >> e;
        if(docid == 1145) {
            break;
        }
    }
    // cout << "s = " << s << "e = " << e << endl;
    ifs2.seekg(s);
    string line2;
    while(getline(ifs2, line2))
    {
        cout << line2 << endl;
        if(ifs2.tellg() == e) break;
    }
    return 0;
}