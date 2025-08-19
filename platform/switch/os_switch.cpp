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
#if defined(OPENGL_ENABLED)
	gl_context->swap_buffers();
#endif
}

Error OS_Switch::initialize(const VideoMode &p_desired, int p_video_driver, int p_audio_driver) {
#if defined(OPENGL_ENABLED)
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
		OS::get_singleton()->alert("Your video card driver does not support any of the supported OpenGL versions.\n"
								   "Please update your drivers or if you have a very old or integrated GPU upgrade it.",
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

	power_manager = memnew(PowerSwitch);

	AudioDriverManager::initialize(p_audio_driver);

	//_ensure_user_data_dir();

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
	memdelete(input);
	memdelete(joypad);
	visual_server->finish();
	memdelete(visual_server);
	memdelete(power_manager);
	memdelete(gl_context);
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
	printf("got alert %ls", p_alert.c_str());
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

OS::Date OS_Switch::get_date(bool local) const {
	return OS::Date();
}

OS::Time OS_Switch::get_time(bool local) const {
	return OS::Time();
}

OS::TimeZoneInfo OS_Switch::get_time_zone_info() const {
	return OS::TimeZoneInfo();
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

bool g_swkbd_open = false;
int g_eat_string_events = 0;
u32 last_len = 0;
s32 last_cursor = 0;

void keyboard_string_changed_callback(const char *str, SwkbdChangedStringArg *arg) {
	// We get a string changed event on appear, and another one on setting text.
	if (g_eat_string_events) {
		last_len = arg->stringLen;
		g_eat_string_events--;
		return;
	}

	if (arg->stringLen < last_len) {
		OS_Switch::get_singleton()->key(KEY_BACKSPACE, true);
	} else if (arg->stringLen != 0) {
		OS_Switch::get_singleton()->key(str[arg->stringLen - 1], true);
	}
	last_len = arg->stringLen;
}

void keyboard_moved_cursor_callback(const char *str, SwkbdMovedCursorArg *arg) {
	if (arg->cursorPos < last_cursor) {
		OS_Switch::get_singleton()->key(KEY_LEFT, true);
	} else {
		OS_Switch::get_singleton()->key(KEY_RIGHT, true);
	}

	last_cursor = arg->cursorPos;
}

void keyboard_decided_enter_callback(const char *str, SwkbdDecidedEnterArg *arg) {
	OS_Switch::get_singleton()->key(KEY_ENTER, true);
	g_swkbd_open = false;
}

void keyboard_decided_cancel_callback() {
	g_swkbd_open = false;
}

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
		TRACE("no main loop???\n");
		return;
	}

	main_loop->init();

	swkbdInlineLaunchForLibraryApplet(&inline_keyboard, SwkbdInlineMode_AppletDisplay, 0);
	swkbdInlineSetChangedStringCallback(&inline_keyboard, keyboard_string_changed_callback);
	swkbdInlineSetMovedCursorCallback(&inline_keyboard, keyboard_moved_cursor_callback);
	swkbdInlineSetDecidedEnterCallback(&inline_keyboard, keyboard_decided_enter_callback);
	swkbdInlineSetDecidedCancelCallback(&inline_keyboard, keyboard_decided_cancel_callback);

	int last_touch_count = 0;
	// maximum of 16 touches
	Vector2 last_touch_pos[16];
	HidTouchScreenState touch_state = { 0 };

	hidInitializeTouchScreen();

	while (appletMainLoop()) {
		if (g_swkbd_open) {
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
		}

		joypad->process();
		input->flush_buffered_events();

		swkbdInlineUpdate(&inline_keyboard, NULL);

		if (Main::iteration())
			break;
	}

	swkbdInlineClose(&inline_keyboard);
	main_loop->finish();
}

bool OS_Switch::has_touchscreen_ui_hint() const {
	return true;
}

bool OS_Switch::has_virtual_keyboard() const {
	return true;
}

int OS_Switch::get_virtual_keyboard_height() const {
	// todo: actually figure this out
	if (!g_swkbd_open) {
		return 0;
	}
	return 300;
}

void OS_Switch::show_virtual_keyboard(const String &p_existing_text, const Rect2 &p_screen_rect, int p_max_input_length) {
	if (!g_swkbd_open) {
		g_swkbd_open = true;

		SwkbdAppearArg appear_arg;
		swkbdInlineMakeAppearArg(&appear_arg, SwkbdType_Normal);
		swkbdInlineSetInputText(&inline_keyboard, p_existing_text.utf8().get_data());
		swkbdInlineSetCursorPos(&inline_keyboard, p_existing_text.size() - 1);

		g_eat_string_events = 2;

		swkbdInlineAppear(&inline_keyboard, &appear_arg);
	}
}

void OS_Switch::hide_virtual_keyboard() {
	printf("Hiding kbd!\n");
	g_swkbd_open = false;
	swkbdInlineDisappear(&inline_keyboard);
}

OS::PowerState OS_Switch::get_power_state() {
	return power_manager->get_power_state();
}

int OS_Switch::get_power_seconds_left() {
	return power_manager->get_power_seconds_left();
}

int OS_Switch::get_power_percent_left() {
	return power_manager->get_power_percent_left();
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
	power_manager = nullptr;
	gl_context = nullptr;
	AudioDriverManager::add_driver(&driver_switch);

	swkbdInlineCreate(&inline_keyboard);
}
