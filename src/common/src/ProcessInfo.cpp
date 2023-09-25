#include "unistd.h"
#include "common/include/ProcessInfo.h"

namespace CppUtil
{

namespace Process
{

pid_t pid()
{
    return ::getpid();
}

}
}