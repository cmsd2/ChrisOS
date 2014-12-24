#include <sys/thread.h>

int idle_thread_start(void);
int idle_thread_func(void * _unused) _Noreturn;
