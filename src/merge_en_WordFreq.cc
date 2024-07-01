#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <map>

using std::endl;
using std::cerr;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::string;
using std::vector;
using std::map;

class WordFrequencyMerger {
public:
    void mergeFiles(const std::vector<string>& inputFiles, const string& outputFile) {
        for (const auto& file : inputFiles) {
            readFile(file);
        }
        store(outputFile);
    }

private:
    map<string, unsigned long> word_cnt;

    void readFile(const string& fileName) {
        ifstream ifs(fileName);
        if (!ifs.good()) {
            cerr << "file " << fileName << " open failed." << endl;
            return;
        }
        
        string line;
        while (getline(ifs, line)) {
            string word;
            unsigned long count;
            istringstream iss(line);
            if (iss >> word >> count) {
                word_cnt[word] += count;
            }
        }

        ifs.close();
    }

    void store(const string& fileName) {
        ofstream ofs(fileName);
        if (!ofs.good()) {
            cerr << "file " << fileName << " open failed." << endl;
            return;
        }

        for (const auto& it : word_cnt) {
            ofs << it.first << "  " << it.second << endl;
        }
        ofs.close();
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <output_file> <input_file1> <input_file2> ..." << endl;
        return 1;
    }

    string outputFile = argv[1];
    std::vector<string> inputFiles;

    for (int i = 2; i < argc; ++i) {
        inputFiles.push_back(argv[i]);
    }

    WordFrequencyMerger merger;
    merger.mergeFiles(inputFiles, outputFile);

    return 0;
}