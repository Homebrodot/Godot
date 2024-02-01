#include "platform_thread.h"
#include "core/object/script_language.h"
#include "core/templates/safe_refcount.h"

#include <coreinit/thread.h>

Thread::PlatformFunctions Thread::platform_functions;

SafeNumeric<uint64_t> Thread::id_counter(1); // The first value after .increment() is 2, hence by default the main thread ID should be 1.

thread_local Thread::ID Thread::caller_id = Thread::UNASSIGNED_ID;

void Thread::PlatformFunctions::init(){
	
}

void Thread::PlatformFunctions::set_name(const String &thr_name){
	char char_name = thr_name.ascii().ptr();
	OSSetThreadName(thread, char_name);
}

void Thread::plat_set_priority(Thread::Priority priority){
    if (priority == Thread::Priority::PRIORITY_HIGH) {
        OSSetThreadPriority(thread, 0);
    }
    else if (settings.priority == Thread::Priority::PRIORITY_NORMAL) {
		OSSetThreadPriority(thread, 16);
    }
    else if (settings.priority == Thread::Priority::PRIORITY_LOW){
        OSSetThreadPriority(thread, 32);
    }
    
}

void Thread::PlatformFunctions::term(){

}

void Thread::PlatformFunctions::wrapper(){

}

void Thread::_set_platform_functions(const PlatformFunctions &p_functions) {
	platform_functions = p_functions;
}

void Thread::callback(ID p_caller_id, const Settings &p_settings, Callback p_callback, void *p_userdata) {
	Thread::caller_id = p_caller_id;
	if (platform_functions.set_priority) {
		platform_functions.set_priority(p_settings.priority);
	}
	if (platform_functions.init) {
		platform_functions.init();
	}
	ScriptServer::thread_enter(); // Scripts may need to attach a stack.
	if (platform_functions.wrapper) {
		platform_functions.wrapper(p_callback, p_userdata);
	} else {
		p_callback(p_userdata);
	}
	ScriptServer::thread_exit();
	if (platform_functions.term) {
		platform_functions.term();
	}
}

Thread::ID Thread::start(Thread::Callback p_callback, void *p_user, const Settings &p_settings) {
	ERR_FAIL_COND_V_MSG(id != UNASSIGNED_ID, UNASSIGNED_ID, "A Thread object has been re-started without wait_to_finish() having been called on it.");
	id = id_counter.increment();
    
	//thread = THREADING_NAMESPACE::thread(&Thread::callback, id, p_settings, p_callback, p_user);

	OSCreateThread(thread,);
    return id;
}

bool Thread::is_started() const {
	return id != UNASSIGNED_ID;
}

void Thread::wait_to_finish() {
	ERR_FAIL_COND_MSG(id == UNASSIGNED_ID, "Attempt of waiting to finish on a thread that was never started.");
	ERR_FAIL_COND_MSG(id == get_caller_id(), "Threads can't wait to finish on themselves, another thread must wait.");
	uint32_t joinresult;
	OSJoinThread(thread, *joinresult)
	//re-init thread?
	//thread = ;
	id = UNASSIGNED_ID;
}

Error Thread::set_name(const String &p_name) {
	if (platform_functions.set_name) {
		return platform_functions.set_name(p_name);
	}

	return ERR_UNAVAILABLE;
}

Thread::~Thread() {
	if (id != UNASSIGNED_ID) {
#ifdef DEBUG_ENABLED
		WARN_PRINT(
				"A Thread object is being destroyed without its completion having been realized.\n"
				"Please call wait_to_finish() on it to ensure correct cleanup.");
#endif
		//thread.detach();
		OSDetachThread(thread);
	}
}