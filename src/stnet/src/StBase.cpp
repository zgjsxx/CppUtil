#include "stnet/include/StBase.h"

//==coroutine create/destroy api====
bool ST_init() {
  int r0 = 0;
  if ((r0 = st_init()) != 0) {
    return false;
  }
  return true;
}

st_thread_t ST_thread_create(void *(*start)(void *arg), void *arg, int joinable,
                             int stk_size) {
  return st_thread_create(start, arg, joinable, stk_size);
}

int ST_thread_join(st_thread_t thread, void **retvalp) {
  return st_thread_join((st_thread_t)thread, retvalp);
}

void ST_thread_exit(void *retval) { ::st_thread_exit(retval); }

void ST_destroy(void) { st_destroy(); }

//======read/write api

ssize_t ST_read(st_netfd_t stfd, void *buf, size_t nbyte, st_utime_t timeout) {
  return st_read(stfd, buf, nbyte, timeout);
}
