
#include "core/templates/safe_refcount.h"
#include "core/typedefs.h"

#ifdef PLATFORM_THREAD_OVERRIDE
#include "platform/wiiu/platform_thread.h"
#else 
#include "core/os/thread.h"
#endif

void wiiu_thread_set_priority(Thread::Settings settings);

void wiiu_thread_set_name(const String thrname);

void wiiu_thread_init();

void wiiu_thread_call_wrap(Thread::Callback, void *);

void wiiu_thread_term();