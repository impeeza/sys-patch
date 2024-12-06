#pragma once

#if defined __cplusplus
extern "C" {
#endif

#include <switch.h>

struct NxFile {
    FsFile file;
    FsFileSystem system;
    s64 offset;
};

#define INI_FILETYPE struct NxFile
#define INI_FILEPOS s64
#define INI_OPENREWRITE
#define INI_REMOVE

bool ini_openread(const char* filename, struct NxFile* nxfile);
bool ini_openwrite(const char* filename, struct NxFile* nxfile);
bool ini_openrewrite(const char* filename, struct NxFile* nxfile);
bool ini_close(struct NxFile* nxfile);
bool ini_read(char* buffer, u64 size, struct NxFile* nxfile);
bool ini_read2(char* buffer, u64 size, struct NxFile* nxfile);
bool ini_write(const char* buffer, struct NxFile* nxfile);
bool ini_tell(struct NxFile* nxfile, s64* pos);
bool ini_seek(struct NxFile* nxfile, s64* pos);
bool ini_rename(const char* src, const char* dst);
bool ini_remove(const char* filename);

#if defined __cplusplus
} // extern "C" {
#endif
