#pragma once

#include "third_party/state-threads-1.9.5/st.h"

namespace CppUtil {
namespace StNet {

bool ST_init();

st_thread_t ST_thread_create(void *(*start)(void *arg), void *arg, int joinable,
                             int stk_size);

int ST_thread_join(st_thread_t thread, void **retvalp);

void ST_thread_exit(void *retval);

void ST_destroy(void);

ssize_t ST_read(st_netfd_t stfd, void *buf, size_t nbyte, st_utime_t timeout);

}  // namespace StNet
}  // namespace CppUtil