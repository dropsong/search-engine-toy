#include "PageLib.h"
#include <iostream>
#include <fstream>
#include <regex>

using std::cerr;
using std::endl;
using std::cout;
using std::ofstream;
using namespace simhash;

void PageLib::Let_Me_See_See(const char* _dir)
{
    XMLDocument doc;
    doc.LoadFile(_dir);
    if(doc.ErrorID()) {
        cerr << "load file failed." << endl;
        return;
    }

    XMLElement* myItemElem = doc.FirstChildElement("rss")
                              -> FirstChildElement("channel")
                              -> FirstChildElement("item");

    if(myItemElem == nullptr) {
        cerr << "this xml file is invalid." << endl;
        return;
    }

    processItem(myItemElem);
        
    while(myItemElem->NextSiblingElement("item")) {
        myItemElem = myItemElem -> NextSiblingElement("item");
        processItem(myItemElem);
    }
}

void PageLib::dump(const string& fileName, int& cnt)
{
    ofstream ofs(fileName, std::ios::app);
    if(!ofs) {
        cerr << "file " << fileName << "open failed." << endl;
        return;
    }

    auto it = _rss.cbegin();
    for(; it != _rss.cend(); it++) {
        cnt ++;
        ofs << "<doc>" << endl
            << "    <docid>" << cnt << "</docid>" << endl
            << "    <title>" << it->title << "</title>" << endl
            << "    <link>" << it->link << "</link>" << endl
            << "    <description>" << it->description << "</description>" << endl
            << "    <content>" << it->content << "</content>" << endl
            << "</doc>" << endl;
    }
    ofs.close();
}

void PageLib::processItem(XMLElement* p)
{
    // get title
    XMLText* myTitleNode = p -> FirstChildElement("title")
                             -> FirstChild() -> ToText();
    string mytitle = myTitleNode -> Value();

    // get link
    XMLText* myLinkNode = p -> FirstChildElement("link")
                             -> FirstChild() -> ToText();
    string mylink = myLinkNode -> Value();

    // get description
    auto myDescriptionNode = p -> FirstChildElement("description");
    std::regex reg("<[^>]+>");
    string mydescription;
    if(myDescriptionNode) {
        auto _myDescriptionNode = myDescriptionNode -> FirstChild() -> ToText();
        mydescription = _myDescriptionNode -> Value();
        mydescription = regex_replace(mydescription, reg, "");
    } else {
        mydescription = "";
    }

    // get content
    // some xml_files have no <content>
    auto myContNode = p -> FirstChildElement("content:encoded");
    if(!myContNode) myContNode = p -> FirstChildElement("content:encoded xml:lang=\"zh-CN\"");
    if(!myContNode) myContNode = p -> FirstChildElement("content type=\"html\"");
    if(!myContNode) myContNode = p -> FirstChildElement("content");

    string mycontent;
    if(myContNode)
    {
        auto _myContNode = myContNode -> FirstChild() -> ToText();
        mycontent = _myContNode -> Value();
        mycontent = regex_replace(mycontent, reg, "");
    } else {
        mycontent = "";
    }

    uint64_t simvalue = calc_simhash(getContent(mydescription, mycontent));
    if(isSimilar(simvalue)) return;

    simhash_values.push_back(simvalue);
    RssItem tmp = (RssItem){mytitle, mylink, mydescription, mycontent};
    _rss.push_back(tmp);
}

bool PageLib::isSimilar(uint64_t x)
{
    for(auto& i : simhash_values)
    {
        // Hamming distance threshold is set to 3 by default
        if(Simhasher::isEqual(i, x, 3)) return true;
    }
    return false;
}

uint64_t PageLib::calc_simhash(const string& s)
{
    size_t topN = 5;
    uint64_t u64 = 0;
    simhasher.make(s, topN, u64);
    return u64;
}

string PageLib::getContent(const string& s1, const string& s2)
{
    if(s2 != "") return s2;
    return s1;
}

int main(int argc, char* argv[])
{
    if(argc < 3) {
        cout << "args error !" << endl;
        cout << "args format:" << endl;
        cout << "<output_file> <input_file1> <input_file2> ..." << endl;
        return -1;
    }

    int cnt = 0;
    PageLib akashi;
    for(int i = 2; argv[i] != 0; i++)
    {
        akashi.Let_Me_See_See(argv[i]);
    }
    akashi.dump(argv[1], cnt);

    return 0;
}