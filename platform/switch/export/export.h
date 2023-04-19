/**************************************************************************/
/*  export.h                                                              */
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

#include <cstring>
#include <string>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;

typedef struct {
	u32 file_off;
	u32 size;
} NsoSegment;

typedef struct {
	u32 unused;
	u32 modOffset;
	u8 padding[8];
} NroStart;

typedef struct {
	u8 magic[4];
	u32 unk1;
	u32 size;
	u32 unk2;
	NsoSegment segments[3];
	u32 bss_size;
	u32 unk3;
	u8 build_id[0x20];
	u8 padding[0x20];
} NroHeader;

typedef struct {
	u64 offset;
	u64 size;
} AssetSection;

typedef struct {
	u8 magic[4];
	u32 version;
	AssetSection icon;
	AssetSection nacp;
	AssetSection romfs;
} AssetHeader;

typedef struct {
	char name[0x200];
	char author[0x100];
} NacpLanguageEntry;

typedef struct {
	NacpLanguageEntry lang[12];
	NacpLanguageEntry lang_unk[4]; //?

	u8 x3000_unk[0x24]; ////Normally all-zero?
	u32 x3024_unk;
	u32 x3028_unk;
	u32 x302C_unk;
	u32 x3030_unk;
	u32 x3034_unk;
	u64 titleid0;

	u8 x3040_unk[0x20];
	char version[0x10];

	u64 titleid_dlcbase;
	u64 titleid1;

	u32 x3080_unk;
	u32 x3084_unk;
	u32 x3088_unk;
	u8 x308C_unk[0x24]; //zeros?

	u64 titleid2;
	u64 titleids[7]; //"Array of application titleIDs, normally the same as the above app-titleIDs. Only set for game-updates?"

	u32 x30F0_unk;
	u32 x30F4_unk;

	u64 titleid3; //"Application titleID. Only set for game-updates?"

	char bcat_passphrase[0x40];
	u8 x3140_unk[0xEC0]; //Normally all-zero?
} NacpStruct;

unsigned char *read_file(const char *fn, size_t *len_out);
unsigned char *read_bytes(const char *fn, size_t off, size_t len);
size_t write_bytes(const char *fn, size_t off, size_t len, const unsigned char *data);

void register_switch_exporter();
