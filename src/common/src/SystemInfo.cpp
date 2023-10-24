#include <stdint.h>
#include "common/include/SystemInfo.h"

namespace CppUtil {
namespace Common {

int getCpuNum() { return sysconf(_SC_NPROCESSORS_ONLN); }

bool isLittleEndian() {
  static int little_endian_check = -1;
  // low addr ==> hight addr
  // |00|00|00|01| big endian
  // |01|00|00|00| little endian

  if (little_endian_check == -1) {
    union {
      int32_t i;
      int8_t c;
    } little_check_union;

    little_check_union.i = 0x01;
    little_endian_check = little_check_union.c;
  }

  return (little_endian_check == 1);
}

}  // namespace Common
}  // namespace CppUtil
