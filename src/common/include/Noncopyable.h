#ifndef CPPUTIL_COMMON_NONCOPYABLE
#define CPPUTIL_COMMON_NONCOPYABLE
namespace CppUtil
{

class Noncopyable
{
public:
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;

protected:
    Noncopyable() = default;
    ~Noncopyable() = default;
};

} //namespace CppUtil

#endif