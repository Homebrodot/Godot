#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <switch.h>

// for getcwd
#include <unistd.h>

#define PACK_HEADER_MAGIC 0x43504447
#define PACK_FORMAT_VERSION 1

typedef struct pack_file_entry {
	char *filename;
	uint64_t size;
	uint64_t offset;
	char md5[16];
} pack_file_entry;

void bail(const char *error)
{
	consoleInit(NULL);
	printf("%s\npress + to exit\n", error);
	padConfigureInput(1, HidNpadStyleSet_NpadStandard);
	PadState pad;
	padInitializeDefault(&pad);

	while (appletMainLoop())
	{
		padUpdate(&pad);
		u64 kDown = padGetButtonsDown(&pad);

		if (kDown & HidNpadButton_Plus)
			break;
		consoleUpdate(NULL);
	}
	consoleExit(NULL);
}

uint32_t read_32(FILE* f) {
	uint32_t n;
	fread(&n, 1, 4, f);
	return n;
}

uint64_t read_64(FILE* f) {
	uint64_t n;
	fread(&n, 1, 8, f);
	return n;
}

struct pack_file_entry* read_file_entry(FILE *f, struct pack_file_entry *entry) {
	uint32_t name_len = read_32(f);
	char *filename = malloc(name_len+1);
	fread(filename, 1, name_len, f);
	filename[name_len] = 0;

	entry->filename = filename;
	entry->offset = read_64(f);
	entry->size = read_64(f);

	fread(&entry->md5, 1, 16, f);
	return entry;
}

bool file_exists(const char *path) {
	FILE *f = fopen(path, "rb");
	if(f != NULL) {
		fclose(f);
		return true;
	}
	return false;
}

void invoke_with_merged_argv(const char *nro_path, int argc, char* argv[]) {
	size_t argv_len = strlen(nro_path) + 1;
	// skip argv[0]
	for(int i = 1; i < argc; i++) {
		argv_len += strlen(argv[i]) + 3; // for start/end quote + space/null term
	}

	char *merged_argv = malloc(argv_len);
	size_t offset = 0;

	// argv[0]
	strcpy(merged_argv, nro_path);
	offset = strlen(nro_path);
	merged_argv[offset++] = ' ';

	for(int i = 1; i < argc; i++) {
		merged_argv[offset++] = '"';
		strcpy(merged_argv + offset, argv[i]);
		offset += strlen(argv[i]);
		merged_argv[offset++] = '"';
		if(i != argc-1) merged_argv[offset++] = ' ';
	}
	merged_argv[offset] = 0;

	printf("launching %s - args '%s'\n", nro_path, merged_argv);

	envSetNextLoad(nro_path, merged_argv);
}

int main(int argc, char* argv[])
{
	socketInitializeDefault();
	nxlinkStdio();

	const char *main_pack = NULL;
	for(int i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "--main-pack") == 0 && i != argc-1) {
			main_pack = argv[i+1];
		}
	}

	if(main_pack) {
		FILE *pack_file = fopen(main_pack, "rb");
		if(pack_file) {
			uint32_t magic = read_32(pack_file);

			if(magic != PACK_HEADER_MAGIC) {
				bail("invalid pck magic!");
			}
			// pack format version, should be 1
			uint32_t version = read_32(pack_file);
			// godot engine version
			uint32_t ver_major = read_32(pack_file);
			uint32_t ver_minor = read_32(pack_file);
			uint32_t ver_patch = read_32(pack_file); // patch number, not used for validation.

			if(version != 1) {
				bail("invalid pck format!");
			}

			printf("pck for godot version %i.%i.%i\n", ver_major, ver_minor, ver_patch);

			// 16 * 4 bytes reserved
			fseek(pack_file, 16*4, SEEK_CUR);

			uint32_t n_files = read_32(pack_file);

			struct pack_file_entry file_entry;
			memset(&file_entry, 0, sizeof(struct pack_file_entry));

			char *custom_editor_id = NULL;
			bool found_custom_editor_id = false;
			// file table
			for(int i = 0; i < n_files; i++) {
				read_file_entry(pack_file, &file_entry);
				printf("got a file %s\n", file_entry.filename);

				if(strcmp(file_entry.filename, "custom_editor_id") == 0) {
					found_custom_editor_id = true;
					custom_editor_id = malloc(file_entry.size + 1);
					fseek(pack_file, file_entry.offset, SEEK_SET);
					fread(custom_editor_id, 1, file_entry.size, pack_file);
					custom_editor_id[file_entry.size] = 0;

					free(file_entry.filename);
					break;
				}
				free(file_entry.filename);
			}

			fclose(pack_file);

			bool found_nro = false;
			char filename_buff[256];
			filename_buff[255]=0;

			char getcwd_buff[100];
			getcwd(getcwd_buff, 100);

			if(found_custom_editor_id) {
				snprintf(filename_buff, 255, "%s/godot-%s.nro", getcwd_buff, custom_editor_id);
				if(file_exists(filename_buff)) {
					invoke_with_merged_argv(filename_buff, argc, argv);
					found_nro = true;
				}
			}
			else {
				// add the cwd here because we need an absolute path later...
				snprintf(filename_buff, 255, "%s/godot-%i.%i.%i.nro", getcwd_buff, ver_major, ver_minor, ver_patch);

				if(file_exists(filename_buff)) {
					invoke_with_merged_argv(filename_buff, argc, argv);
					found_nro = true;
				}
				else {
					int patch = ver_patch + 1;
					while(patch--) {
						if(patch != 0) {
							snprintf(filename_buff, 255, "%s/godot-%i.%i.%i.nro", getcwd_buff, ver_major, ver_minor, patch);
						}
						else {
							snprintf(filename_buff, 255, "%s/godot-%i.%i.nro", getcwd_buff, ver_major, ver_minor);
						}

						if(file_exists(filename_buff)) {
							invoke_with_merged_argv(filename_buff, argc, argv);
							found_nro = true;
							break;
						}
					}
				}
			}

			if(!found_nro) {
				char bail_reason[256];
				bail_reason[255]=0;
				if(found_custom_editor_id) {
					snprintf(bail_reason, 255, "Failed to find a compatible Godot version, wanted a custom build '%s'!", custom_editor_id);
				}
				else {
					snprintf(bail_reason, 255, "Failed to find a compatible Godot version, wanted %i.%i.%i!", ver_major, ver_minor, ver_patch);
				}
				bail(bail_reason);
			}

			if(found_custom_editor_id) free(custom_editor_id);
		}
		else {
			bail("Failed to open PCK!");
		}
	}
	else {
		bail("Failed to find main_pack argument!");
	}

	fflush(stdout);
	fflush(stderr);

	socketExit();

	return 0;
}
