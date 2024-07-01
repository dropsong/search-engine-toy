#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

class NonCopyable
{
public:
    NonCopyable() {}
    ~NonCopyable() {}

    //将拷贝构造函数与赋值运算符函数删除了
    NonCopyable(const NonCopyable &rhs) = delete;
    NonCopyable &operator=(const NonCopyable &rhs) = delete;

private:

};

#endif
