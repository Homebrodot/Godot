/**************************************************************************/
/*  os_switch.cpp                                                         */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "os_switch.h"
#include "context_gl_switch_egl.h"
#include "switch_wrapper.h"

#include "drivers/gles2/rasterizer_gles2.h"
#include "drivers/gles3/rasterizer_gles3.h"
#include "drivers/unix/dir_access_unix.h"
#include "drivers/unix/file_access_unix.h"
#include "drivers/unix/ip_unix.h"
#include "drivers/unix/net_socket_posix.h"
#include "drivers/unix/thread_posix.h"
#include "main/main.h"
#include "servers/audio_server.h"
#include "servers/visual/visual_server_wrap_mt.h"

#include "core/os/keyboard.h"

#include <inttypes.h>
#include <netinet/in.h>
#include <stdio.h>

#define ENABLE_NXLINK

#ifndef ENABLE_NXLINK
#define TRACE(fmt, ...) ((void)0)
#else
#include <unistd.h>
#define TRACE(fmt, ...) printf("%s: " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

void OS_Switch::initialize_core() {
#if !defined(NO_THREADS)
	init_thread_posix();
#endif

	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_RESOURCES);
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_USERDATA);
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_FILESYSTEM);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_RESOURCES);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_USERDATA);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_FILESYSTEM);

#ifndef NO_NETWORK
	NetSocketPosix::make_default();
	IP_Unix::make_default();
#endif
}

void OS_Switch::swap_buffers() {
#ifdef OPENGL_ENABLED
	gl_context->swap_buffers();
#endif
}

Error OS_Switch::initialize(const VideoMode &p_desired, int p_video_driver, int p_audio_driver) {
#ifdef OPENGL_ENABLED
	bool gles3_context = true;
	if (p_video_driver == VIDEO_DRIVER_GLES2) {
		gles3_context = false;
	}

	bool editor = Engine::get_singleton()->is_editor_hint();
	bool gl_initialization_error = false;

	gl_context = NULL;
	while (!gl_context) {
		gl_context = memnew(ContextGLSwitchEGL(gles3_context));

		if (gl_context->initialize() != OK) {
			memdelete(gl_context);
			gl_context = NULL;

			if (GLOBAL_GET("rendering/quality/driver/fallback_to_gles2") || editor) {
				if (p_video_driver == VIDEO_DRIVER_GLES2) {
					gl_initialization_error = true;
					break;
				}

				p_video_driver = VIDEO_DRIVER_GLES2;
				gles3_context = false;
			} else {
				gl_initialization_error = true;
				break;
			}
		}
	}

	while (true) {
		if (gles3_context) {
			if (RasterizerGLES3::is_viable() == OK) {
				RasterizerGLES3::register_config();
				RasterizerGLES3::make_current();
				break;
			} else {
				if (GLOBAL_GET("rendering/quality/driver/fallback_to_gles2") || editor) {
					p_video_driver = VIDEO_DRIVER_GLES2;
					gles3_context = false;
					continue;
				} else {
					gl_initialization_error = true;
					break;
				}
			}
		} else {
			if (RasterizerGLES2::is_viable() == OK) {
				RasterizerGLES2::register_config();
				RasterizerGLES2::make_current();
				break;
			} else {
				gl_initialization_error = true;
				break;
			}
		}
	}

	if (gl_initialization_error) {
		OS::get_singleton()->alert("Your firmware does not support any of the supported OpenGL versions.\n"
								   "Please update your custom firmware to install the latest OpenGL driver.",
				"Unable to initialize Video driver");
		return ERR_UNAVAILABLE;
	}

	video_driver_index = p_video_driver;

	gl_context->set_use_vsync(current_videomode.use_vsync);
#endif

	visual_server = memnew(VisualServerRaster);
	if (get_render_thread_mode() != RENDER_THREAD_UNSAFE) {
		visual_server = memnew(VisualServerWrapMT(visual_server, get_render_thread_mode() == RENDER_SEPARATE_THREAD));
	}

	visual_server->init();

	input = memnew(InputDefault);
	input->set_emulate_mouse_from_touch(true);
	// TODO: handle joypads/joycons status
	for (int i = 0; i < 8; i++) {
		input->joy_connection_changed(i, true, "pad" + (char)i, "");
	}
	joypad = memnew(JoypadSwitch(input));

	if (R_SUCCEEDED(psmInitialize())) {
		OS_Switch::psm_initialized = true;
	}

	AudioDriverManager::initialize(p_audio_driver);

	return OK;
}

void OS_Switch::set_main_loop(MainLoop *p_main_loop) {
	input->set_main_loop(p_main_loop);
	main_loop = p_main_loop;
}

void OS_Switch::delete_main_loop() {
	if (main_loop)
		memdelete(main_loop);
	main_loop = NULL;
}

void OS_Switch::finalize() {
	NintendoSwitch::get_singleton()->cleanup();

	memdelete(input);
	memdelete(joypad);
	visual_server->finish();
	memdelete(visual_server);
	memdelete(gl_context);
	psmExit();
}

void OS_Switch::finalize_core() {
}

bool OS_Switch::_check_internal_feature_support(const String &p_feature) {
	if (p_feature == "mobile") {
		//TODO support etc2 only if GLES3 driver is selected
		return true;
	}
	return false;
}

void OS_Switch::alert(const String &p_alert, const String &p_title) {
	ErrorApplicationConfig config;
	errorApplicationCreate(&config, p_title.utf8().ptr(), p_alert.utf8().ptr());
	errorApplicationShow(&config);
}
String OS_Switch::get_stdin_string(bool p_block) {
	return "";
}
Point2 OS_Switch::get_mouse_position() const {
	return Point2(0, 0);
}

int OS_Switch::get_mouse_button_state() const {
	return 0;
}

void OS_Switch::set_window_title(const String &p_title) {}

void OS_Switch::set_video_mode(const OS::VideoMode &p_video_mode, int p_screen) {}
OS::VideoMode OS_Switch::get_video_mode(int p_screen) const {
	return VideoMode(1280, 720);
}

void OS_Switch::get_fullscreen_mode_list(List<OS::VideoMode> *p_list, int p_screen) const {}

OS::RenderThreadMode OS_Switch::get_render_thread_mode() const {
	if (OS::get_render_thread_mode() == OS::RenderThreadMode::RENDER_SEPARATE_THREAD) {
		return OS::RENDER_THREAD_SAFE;
	}
	return OS::get_render_thread_mode();
}

int OS_Switch::get_current_video_driver() const {
	return video_driver_index;
}
Size2 OS_Switch::get_window_size() const {
	return Size2(1280, 720);
}

Error OS_Switch::execute(const String &p_path, const List<String> &p_arguments, bool p_blocking, ProcessID *r_child_id, String *r_pipe, int *r_exitcode, bool read_stderr, Mutex *p_pipe_mutex, bool p_open_console) {
	if (p_blocking) {
		return FAILED; // we don't support this
	}

	Vector<String> rebuilt_arguments;
	rebuilt_arguments.push_back(p_path); // !!!! v important
	// This is a super dumb implementation to make the editor vaguely work.
	// It won't work if you don't exit afterwards.
	for (const List<String>::Element *E = p_arguments.front(); E; E = E->next()) {
		if ((*E)->find(" ") >= 0) {
			rebuilt_arguments.push_back(String("\"") + E->get() + String("\""));
		} else {
			rebuilt_arguments.push_back(E->get());
		}
	}

	if (__nxlink_host.s_addr != 0) {
		char nxlinked[17];
		sprintf(nxlinked, "%08" PRIx32 "_NXLINK_", __nxlink_host.s_addr);
		rebuilt_arguments.push_back(nxlinked);
	}
	envSetNextLoad(p_path.utf8().ptr(), String(" ").join(rebuilt_arguments).utf8().ptr());
	return OK;
}

Error OS_Switch::kill(const ProcessID &p_pid) {
	return FAILED;
}

bool OS_Switch::is_process_running(const ProcessID &p_pid) const {
	return false;
}

bool OS_Switch::has_environment(const String &p_var) const {
	return false;
}

String OS_Switch::get_environment(const String &p_var) const {
	return "";
}

bool OS_Switch::set_environment(const String &p_var, const String &p_value) const {
	return false;
}

String OS_Switch::get_name() const {
	return "Switch";
}

MainLoop *OS_Switch::get_main_loop() const {
	return main_loop;
}

/// From os_unix.cpp
OS::Date OS_Switch::get_date(bool utc) const {
	time_t t = time(nullptr);
	struct tm lt;
	if (utc) {
		gmtime_r(&t, &lt);
	} else {
		localtime_r(&t, &lt);
	}
	Date ret;
	ret.year = 1900 + lt.tm_year;
	// Index starting at 1 to match OS_Unix::get_date
	//   and Windows SYSTEMTIME and tm_mon follows the typical structure
	//   of 0-11, noted here: http://www.cplusplus.com/reference/ctime/tm/
	ret.month = (Month)(lt.tm_mon + 1);
	ret.day = lt.tm_mday;
	ret.weekday = (Weekday)lt.tm_wday;
	ret.dst = lt.tm_isdst;

	return ret;
}

/// From os_unix.cpp
OS::Time OS_Switch::get_time(bool utc) const {
	time_t t = time(nullptr);
	struct tm lt;
	if (utc) {
		gmtime_r(&t, &lt);
	} else {
		localtime_r(&t, &lt);
	}
	Time ret;
	ret.hour = lt.tm_hour;
	ret.min = lt.tm_min;
	ret.sec = lt.tm_sec;
	get_time_zone_info();
	return ret;
}

/// From os_unix.cpp
OS::TimeZoneInfo OS_Switch::get_time_zone_info() const {
	time_t t = time(nullptr);
	struct tm lt;
	localtime_r(&t, &lt);
	char name[16];
	strftime(name, 16, "%Z", &lt);
	name[15] = 0;
	TimeZoneInfo ret;
	ret.name = name;

	char bias_buf[16];
	strftime(bias_buf, 16, "%z", &lt);
	int bias;
	bias_buf[15] = 0;
	sscanf(bias_buf, "%d", &bias);

	// convert from ISO 8601 (1 minute=1, 1 hour=100) to minutes
	int hour = (int)bias / 100;
	int minutes = bias % 100;
	if (bias < 0) {
		ret.bias = hour * 60 - minutes;
	} else {
		ret.bias = hour * 60 + minutes;
	}

	return ret;
}

void OS_Switch::delay_usec(uint32_t p_usec) const {
	svcSleepThread((int64_t)p_usec * 1000ll);
}

uint64_t OS_Switch::get_ticks_usec() const {
	static u64 tick_freq = armGetSystemTickFreq();
	return armGetSystemTick() / (tick_freq / 1000000);
}

bool OS_Switch::can_draw() const {
	return true;
}

void OS_Switch::set_cursor_shape(CursorShape p_shape) {}
void OS_Switch::set_custom_mouse_cursor(const RES &p_cursor, CursorShape p_shape, const Vector2 &p_hotspot) {}

void OS_Switch::key(uint32_t p_key, bool p_pressed) {
	Ref<InputEventKey> ev;
	ev.instance();
	ev->set_echo(false);
	ev->set_pressed(p_pressed);
	ev->set_scancode(p_key);
	ev->set_unicode(p_key);
	input->parse_input_event(ev);
};

void OS_Switch::run() {
	if (!main_loop) {
		TRACE("No main loop?\n");
		return;
	}

	main_loop->init();

	NintendoSwitch::get_singleton()->initialize_software_keyboard();

	int last_touch_count = 0;
	// maximum of 16 touches
	Vector2 last_touch_pos[16];
	HidTouchScreenState touch_state = { 0 };

	hidInitializeTouchScreen();

	while (appletMainLoop()) {
		if (NintendoSwitch::get_singleton()->is_virtual_keyboard_open()) {
			for (int i = 0; i < last_touch_count; i++) {
				Ref<InputEventScreenTouch> st;
				st.instance();
				st->set_index(i);
				st->set_position(last_touch_pos[i]);
				st->set_pressed(false);
				input->parse_input_event(st);
			}

			last_touch_count = 0;
		} else {
			if (hidGetTouchScreenStates(&touch_state, 1)) {
				if (touch_state.count != last_touch_count) {
					// gained new touches, add them
					if (touch_state.count > last_touch_count) {
						for (int i = last_touch_count; i < touch_state.count; i++) {
							Vector2 pos(touch_state.touches[i].x, touch_state.touches[i].y);

							Ref<InputEventScreenTouch> st;
							st.instance();
							st->set_index(i);
							st->set_position(pos);
							st->set_pressed(true);
							input->parse_input_event(st);
						}
					} else // lost touches
					{
						for (int i = touch_state.count; i < last_touch_count; i++) {
							Ref<InputEventScreenTouch> st;
							st.instance();
							st->set_index(i);
							st->set_position(last_touch_pos[i]);
							st->set_pressed(false);
							input->parse_input_event(st);
						}
					}
				} else {
					for (int i = 0; i < touch_state.count; i++) {
						Vector2 pos(touch_state.touches[i].x, touch_state.touches[i].y);

						Ref<InputEventScreenDrag> sd;
						sd.instance();
						sd->set_index(i);
						sd->set_position(pos);
						sd->set_relative(pos - last_touch_pos[i]);
						last_touch_pos[i] = pos;
						input->parse_input_event(sd);
					}
				}

				last_touch_count = touch_state.count;
			}

			joypad->process();
			input->flush_buffered_events();
		}

		NintendoSwitch::get_singleton()->update();

		if (Main::iteration())
			break;
	}

	main_loop->finish();
}

bool OS_Switch::has_touchscreen_ui_hint() const {
	return true;
}

bool OS_Switch::has_virtual_keyboard() const {
	return true;
}

int OS_Switch::get_virtual_keyboard_height() const {
	if (!NintendoSwitch::get_singleton()->is_virtual_keyboard_open()) {
		return 0;
	}
	return 400;
}

void OS_Switch::show_virtual_keyboard(const String &p_existing_text, const Rect2 &p_screen_rect, bool p_multiline, int p_max_input_length, int p_cursor_start, int p_cursor_end) {
	NintendoSwitch::get_singleton()->show_virtual_keyboard(p_existing_text, NintendoSwitch::NORMAL_KEYBOARD);
}

void OS_Switch::hide_virtual_keyboard() {
	NintendoSwitch::get_singleton()->hide_virtual_keyboard();
}

OS::PowerState OS_Switch::get_power_state() {
	if (!OS_Switch::psm_initialized) {
		return OS::POWERSTATE_UNKNOWN;
	}

	bool enough_power;
	psmIsEnoughPowerSupplied(&enough_power);

	if (!enough_power) {
		return OS::PowerState::POWERSTATE_ON_BATTERY;
	}

	int percentage = OS_Switch::get_power_percent_left();

	if (percentage == 100) {
		return OS::PowerState::POWERSTATE_CHARGED;
	}

	return OS::PowerState::POWERSTATE_CHARGING;
}

int OS_Switch::get_power_seconds_left() {
	WARN_PRINT("power_seconds_left is not implemented on this platform, defaulting to -1");
	return -1;
}

int OS_Switch::get_power_percent_left() {
	if (!OS_Switch::psm_initialized) {
		return -1;
	}

	u32 voltage_percentage;
	psmGetBatteryChargePercentage(&voltage_percentage);
	return (int)voltage_percentage;
}

String OS_Switch::get_executable_path() const {
	return switch_execpath;
}

void OS_Switch::set_executable_path(const char *p_execpath) {
	switch_execpath = p_execpath;
}

String OS_Switch::get_data_path() const {
	return "sdmc:/switch";
}

String OS_Switch::get_user_data_dir() const {
	String appname = get_safe_dir_name(ProjectSettings::get_singleton()->get("application/config/name"));
	if (appname != "") {
		bool use_custom_dir = ProjectSettings::get_singleton()->get("application/config/use_custom_user_dir");
		if (use_custom_dir) {
			String custom_dir = get_safe_dir_name(ProjectSettings::get_singleton()->get("application/config/custom_user_dir_name"), true);
			if (custom_dir == "") {
				custom_dir = appname;
			}
			return get_data_path().plus_file(custom_dir);
		} else {
			return get_data_path().plus_file(get_godot_dir_name()).plus_file("app_userdata").plus_file(appname);
		}
	}
	return get_data_path().plus_file(get_godot_dir_name()).plus_file("app_userdata").plus_file("__unknown");
}

OS_Switch *OS_Switch::get_singleton() {
	return (OS_Switch *)OS::get_singleton();
};

OS_Switch::OS_Switch() {
	video_driver_index = 0;
	main_loop = nullptr;
	visual_server = nullptr;
	input = nullptr;
	gl_context = nullptr;

	AudioDriverManager::add_driver(&driver_audren);
}
