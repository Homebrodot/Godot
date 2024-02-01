//WUT stuff
#include <whb/proc.h>
#include <whb/gfx.h>
#include <padscore/kpad.h>
#include <vpad/input.h>

//platform files
#include "os_wiiu.h"
#include "processor/wiiu_thread.h"
#include "graphics/CafeGLSLCompiler.h"

#ifndef PLATFORM_THREAD_OVERRIDE
#include "core/os/thread.h"
#else 
#include "platform/wiiu/platform_thread.h"
#endif

int main(int argc, char *argv) {
    OS_WiiU os;
    
    //Setting the special platform thread functions
    Thread::PlatformFunctions customfuncs;
    customfuncs.init = wiiu_thread_init;
    customfuncs.set_name = wiiu_thread_set_name;
    customfuncs.set_priority = wiiu_thread_set_priority;
    customfuncs.wrapper = wiiu_thread_call_wrap;
    customfuncs.term = wiiu_thread_term;
    Thread::_set_platform_functions(customfuncs)

    //Start the Wii U stuff
    WHBProcInit();
	WhbGfxInit();
	InitGLSLCompiler();
	VPADInit(); //idc if it's depreciated
	KPADInit();



	
    //Basically copy pasted from  godot_linuxbsd.cpp's main function
	char *cwd = (char *)malloc(PATH_MAX);
	ERR_FAIL_NULL_V(cwd, ERR_OUT_OF_MEMORY);
	char *ret = getcwd(cwd, PATH_MAX);
	//Set up Godot
	Error err = Main::setup(argv[0], argc - 1, &argv[1]);
	if (err != OK) {
		free(cwd);
		if (err == ERR_HELP) { // Returned by --help and --version, so success.
			return 0;
		}
		return 255;
	}
	//Run the Godot-integrated main loop
	if (Main::start()) {
		os.set_exit_code(EXIT_SUCCESS);
		os.run(); // This is the REAL main loop
	}
	Main::cleanup();



	return os.get_exit_code();
}