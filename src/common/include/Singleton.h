#ifndef CPPUTIL_COMMON_SINGLETON_H
#define CPPUTIL_COMMON_SINGLETON_H

#include <Noncopyable.h>

namespace CppUtil
{

template<typename T>
class Singleton : public Noncopyable
{
public:
    Singleton() = delete;
    ~Singleton() = delete;

    static T& instance()
    {
        pthread_once(&ponce, &Singleton::init);
        assert(value_ != NULL);
        return *value;
    }
private:
    static void init()
    {
        value_ = new T();
        ::atexit(destroy);
    }

    static void destroy()
    {
        typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
        T_must_be_complete_type dummy; 
        (void) dummy;

        delete value_;
        value = nullptr;
    }

    static pthread_once_t ponce_;
    static T*             value_;
};

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

} //namespace CppUtil

#endif // CPPUTIL_COMMON_SINGLETON_H