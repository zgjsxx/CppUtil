#include "common/include/SystemInfo.h"

namespace CppUtil {
namespace Common {
int getCpuNum() { return sysconf(_SC_NPROCESSORS_ONLN); }
}  // namespace Common
}  // namespace CppUtil
