#include "useRedis.h"
#include "myRedis.h"
#include <memory>

using std::unique_ptr;

string findInCache(const string& A, string flag)
{
    unique_ptr<MyRedis> pRedis(new MyRedis());
    if(!pRedis->connect("127.0.0.1", 6379)){
        cerr << "connect error ! " << endl;
    }

    return pRedis->get("flAg"+flag+"xD"+A);
}

void putInCache(const string& A, const string& B, string flag)
{
    unique_ptr<MyRedis> pRedis(new MyRedis());
    if(!pRedis->connect("127.0.0.1", 6379)){
        cerr << "connect error ! " << endl;
        return;
    }

    pRedis->set("flAg"+flag+"xD"+A, B);
}
