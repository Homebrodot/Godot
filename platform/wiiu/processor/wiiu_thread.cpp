#include <coreinit/thread.h>

#include "wiiu_thread.h"

#ifndef PLATFORM_THREAD_OVERRIDE
#include "core/os/thread.h"
#else 
#include "platform/wiiu/platform_thread.h"

void wiiu_thread_set_name(const String thrname){
	char char_name = thr_name.ascii().ptr();
	OSSetThreadName(thread, char_name);
}

void wiiu_thread_set_priority(Thread::Settings settings){
    
    if (settings.priority == Thread::Priority::PRIORITY_HIGH) {
        OSSetThreadPriority()
    }
    else if (settings.priority == Thread::Priority::PRIORITY_NORMAL) {

    }
    else if (settings.priority == Thread::Priority::PRIORITY_LOW){
        
    }
    
}

void wiiu_thread_init(){

}

void wiiu_thread_call_wrap(Thread::Callback, void *){

}

void wiiu_thread_term(){

}