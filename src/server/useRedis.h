#ifndef _USEREDIS_H_
#define _USEREDIS_H_

#include <iostream>
#include <string>

using std::cerr;
using std::endl;
using std::string;

string findInCache(const string& A, string flag);
void putInCache(const string& A, const string& B, string flag);

#endif
