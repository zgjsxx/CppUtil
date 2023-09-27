#ifndef CPPUTIL_NET_CHANNEL_H
#define CPPUTIL_NET_CHANNEL_H

#include "common/include/Noncopyable.h"

class Channel : public Noncopyable 
{
public:
private:
    int fd_;
    int events;
    int revent;//ready event
};


#endif