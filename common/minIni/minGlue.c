#include "minGlue.h"
#include <string.h>

static bool ini_open(const char* filename, struct NxFile* nxfile, u32 mode) {
    Result rc = {0};
    char filename_buf[FS_MAX_PATH] = {0};

    if (R_FAILED(rc = fsOpenSdCardFileSystem(&nxfile->system))) {
        return false;
    }

    strcpy(filename_buf, filename);

    if (R_FAILED(rc = fsFsOpenFile(&nxfile->system, filename_buf, mode, &nxfile->file))) {
        if (mode & FsOpenMode_Write) {
            if (R_FAILED(rc = fsFsCreateFile(&nxfile->system, filename_buf, 0, 0))) {
                fsFsClose(&nxfile->system);
                return false;
            } else {
                if (R_FAILED(rc = fsFsOpenFile(&nxfile->system, filename_buf, mode, &nxfile->file))) {
                    fsFsClose(&nxfile->system);
                    return false;
                }
            }
        } else {
            fsFsClose(&nxfile->system);
            return false;
        }
    }

    nxfile->offset = 0;
    return true;
}

bool ini_openread(const char* filename, struct NxFile* nxfile) {
    return ini_open(filename, nxfile, FsOpenMode_Read);
}

bool ini_openwrite(const char* filename, struct NxFile* nxfile) {
    return ini_open(filename, nxfile, FsOpenMode_Write|FsOpenMode_Append);
}

bool ini_openrewrite(const char* filename, struct NxFile* nxfile) {
    return ini_open(filename, nxfile, FsOpenMode_Read|FsOpenMode_Write|FsOpenMode_Append);
}

bool ini_close(struct NxFile* nxfile) {
    fsFileClose(&nxfile->file);
    fsFsClose(&nxfile->system);
    return true;
}

bool ini_read(char* buffer, u64 size, struct NxFile* nxfile) {
    u64 bytes_read = {0};
    if (R_FAILED(fsFileRead(&nxfile->file, nxfile->offset, buffer, size, FsReadOption_None, &bytes_read))) {
        return false;
    }

    if (!bytes_read) {
        return false;
    }

    char *eol = {0};

    if ((eol = strchr(buffer, '\n')) == NULL) {
        eol = strchr(buffer, '\r');
    }

    if (eol != NULL) {
        *++eol = '\0';
        bytes_read = eol - buffer;
    }

    nxfile->offset += bytes_read;
    return true;
}

bool ini_read2(char* buffer, u64 size, struct NxFile* nxfile) {
    u64 bytes_read = 0;

    if (R_FAILED(fsFileRead(&nxfile->file, nxfile->offset, buffer, size - 1, FsReadOption_None, &bytes_read))) {
        return false;
    }

    if (bytes_read == 0) {
        return false;
    }

    buffer[bytes_read] = '\0';

    char* eol = strchr(buffer, '\n');
    if (!eol) {
        eol = strchr(buffer, '\r');
    }

    if (eol) {
        *eol = '\0';
        nxfile->offset += (eol - buffer + 1);
    } else {
        nxfile->offset += bytes_read;
        return true;
    }

    return true;
}

bool ini_write(const char* buffer, struct NxFile* nxfile) {
    const size_t size = strlen(buffer);
    if (R_FAILED(fsFileWrite(&nxfile->file, nxfile->offset, buffer, size, FsWriteOption_None))) {
        return false;
    }
    nxfile->offset += size;
    return true;
}

bool ini_tell(struct NxFile* nxfile, s64* pos) {
    *pos = nxfile->offset;
    return true;
}

bool ini_seek(struct NxFile* nxfile, s64* pos) {
    nxfile->offset = *pos;
    return true;
}

bool ini_rename(const char* src, const char* dst) {
    Result rc = {0};
    FsFileSystem fs = {0};
    char src_buf[FS_MAX_PATH] = {0};
    char dst_buf[FS_MAX_PATH] = {0};

    if (R_FAILED(rc = fsOpenSdCardFileSystem(&fs))) {
        return false;
    }

    strcpy(src_buf, src);
    strcpy(dst_buf, dst);
    rc = fsFsRenameFile(&fs, src_buf, dst_buf);
    fsFsClose(&fs);
    return R_SUCCEEDED(rc);
}

bool ini_remove(const char* filename) {
    Result rc = {0};
    FsFileSystem fs = {0};
    char filename_buf[FS_MAX_PATH] = {0};

    if (R_FAILED(rc = fsOpenSdCardFileSystem(&fs))) {
        return false;
    }

    strcpy(filename_buf, filename);
    rc = fsFsDeleteFile(&fs, filename_buf);
    fsFsClose(&fs);
    return R_SUCCEEDED(rc);
}
