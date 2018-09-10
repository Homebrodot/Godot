/**************************************************************************/
/*  godot_switch.cpp                                                      */
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

#include "switch_wrapper.h"
#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>

#include "main/main.h"
#include "os_switch.h"

#define FB_WIDTH 1280
#define FB_HEIGHT 720

int main(int argc, char *argv[]) {
	socketInitializeDefault();
	nxlinkStdio();

	romfsInit();

	int apptype = appletGetAppletType();
	if (apptype != AppletType_Application && apptype != AppletType_SystemApplication) {
		NWindow *win = nwindowGetDefault();
		Framebuffer fb;
		framebufferCreate(&fb, win, FB_WIDTH, FB_HEIGHT, PIXEL_FORMAT_RGBA_8888, 1);
		framebufferMakeLinear(&fb);

		u32 stride;
		u32 *framebuf = (u32 *)framebufferBegin(&fb, &stride);

		FILE *splash = fopen("romfs:/applet_splash.rgba.gz", "rb");
		if (splash) {
			fseek(splash, 0, SEEK_END);
			size_t splash_size = ftell(splash);

			u8 *compressed_splash = (u8 *)malloc(splash_size);
			memset(compressed_splash, 0, splash_size);
			fseek(splash, 0, SEEK_SET);

			fread(compressed_splash, 1, splash_size, splash);
			fclose(splash);

			memset(framebuf, 0, stride * FB_HEIGHT);

			struct z_stream_s stream;
			memset(&stream, 0, sizeof(stream));
			stream.zalloc = NULL;

			stream.zfree = NULL;
			stream.next_in = compressed_splash;
			stream.avail_in = splash_size;
			stream.next_out = (u8 *)framebuf;
			stream.avail_out = stride * FB_HEIGHT;

			if (inflateInit2(&stream, 16 + MAX_WBITS) == Z_OK) {
				int err = 0;
				if ((err = inflate(&stream, 0)) != Z_STREAM_END) {
					// idk
				}

				inflateEnd(&stream);
			}
		} else {
			// this REALLY shouldn't fail. Hm.
		}

		framebufferEnd(&fb);

		// set up input
		PadState pad;
		padConfigureInput(1, HidNpadStyleSet_NpadStandard);
		padInitializeDefault(&pad);
		while (appletMainLoop()) {
			padUpdate(&pad);
			if (padGetButtonsDown(&pad) != 0) {
				break;
			}
		}

		framebufferClose(&fb);

		romfsExit();
		socketExit();

		return 0;
	}

	OS_Switch os;
	os.set_executable_path(argv[0]);

	char *cwd = (char *)malloc(PATH_MAX);
	getcwd(cwd, PATH_MAX);

	Error err = Main::setup(argv[0], argc - 1, &argv[1]);
	if (err != OK) {
		free(cwd);

		socketExit();
		return 255;
	}

	if (Main::start()) {
		os.run(); // it is actually the OS that decides how to run
	}
	Main::cleanup();

	chdir(cwd);
	free(cwd);

	romfsExit();
	socketExit();
	return os.get_exit_code();
}
