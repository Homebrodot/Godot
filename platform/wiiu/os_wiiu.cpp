#include "os_wiiu.h"

#include "core/debugger/engine_debugger.h"
#include "core/debugger/script_debugger.h"
#include "core/io/marshalls.h"
#include "core/version_generated.gen.h"
// #include "fileaccess/dir_access_wiiu.h" //placeholder
// #include "fileaccess/file_access_wiiu.h" //placeholder
#include "main/main.h"
#include "servers/audio_server.h"
#include "servers/rendering/rendering_server_default.h"
#include "servers/text_server.h"
#include "core/variant/typed_array.h"

//For the fancy native Wii U errors
#include <nn/erreula.h>

//For system info
#include <coreinit/systeminfo.h>
#include <coreinit/mcp.h>
#include <coreinit/time.h>

//Temporary until fileaccess is implemented?
#include <coreinit/filesystem.h>

void OS_WiiU::alert(const String &p_alert, const String &p_title) {
    //Not sure what this FSClient stuff does but it's important
    //TODO: Memory allocation in a Godot-native way
    FSClient *fsClient = (FSClient *) //something memory allocation
    FSAddClient(*fsClient, FS_ERROR_OK)

    nn::erreula::CreateArg newargument;
    newargument.region = nn::erreula::RegionType::USA;
    newargument.language = nn:erreula::LangType::English;
    //newargument.workMemory = //something memory allocation
    newargument.fsClient = *fsClient

    nn::erreula::AppearArg errorbubble;
    errorbubble.errorArg.errorType = nn::erreula::ErrorType::Message1Button;
    errorbubble.errorArg.renderTarget = nn::erreula::RenderTarget::Both;
    //To-do: Have it ask for whichever controller is P1
    errorbubble.errorArg.errorMessage = p_alert;
    errorbubble.errorArg.button1Label = "Ok";
    errorbubble.errorArg.errorTitle = p_title;
    nn::erreula::AppearErrorViewer(errorbubble);
    //I think the rest of the "cleanup" code is supposed to go elsewhere, like a mainloop or something
}

String OS_WiiU::get_name() const {
	return "Wii U";
}

String OS_WiiU::get_distribution_name() const {
	return "Cafe OS";
}

String OS_WiiU::get_version() const {
    //Still dunno what's going on here
    MCPSystemVersion returnthis;
    uint32_t param1;
    MCP_GetSystemVersion(param1, *returnthis);
    return "5.5.5"; 
}

String OS_WiiU::get_processor_name() const {
    return "Espresso";
}

PackedStringArray OS_WiiU::get_video_adapter_driver_info() const {
    PackedStringArray info;
    info.append("amd");
    info.append("1.0");
    return info;
}

Error OS_WiiU::shell_open(String uri) {
    //Implement check for http, open in Internet Browser if found
    //Implement for some sort of titleid handle, for launching apps
    return ERR_METHOD_NOT_FOUND
}

double OS_WiiU::get_unix_time() {
    OSCalendarTime thetime;
    thetime = OSGetTime();
    
}

void OS_WiiU::run(){
	if (!main_loop) {
		return;
	}

	main_loop->initialize();

	//uint64_t last_ticks=get_ticks_usec();

	//int frames=0;
	//uint64_t frame=0;

	while (true) {
		DisplayServer::get_singleton()->process_events(); // get rid of pending events
		input_wiiu->process_inputs();


		if (Main::iteration()) {
			break;
		}
    
}