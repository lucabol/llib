#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dirent.h>
#endif

#include "portable.h"

#include "dir.h"
#include "str.h"
#include "mem.h"
#include "utf8.h"
#include "assert.h"
#include "safeint.h"

#ifdef _WIN32
#define DIR_PATH_EXTRA 2
#define DIR_PATH_MAX 256
#define DIR_FILENAME_MAX 256
#else
#define DIR_PATH_EXTRA 0
#define DIR_PATH_MAX 4096
#define DIR_FILENAME_MAX 256
#endif

#define T dir_entry

struct T
{
#ifdef _WIN32
    HANDLE _d;
    char* first_name;
#else
    DIR* _d;
#endif
};

const Except_T Dir_entry_error = { "Error getting a directory entry"};

T Dir_open(const char *path)
{
    T dir;

    assert (path != NULL || strlen(path) != 0);
    assert (strlen(path) + DIR_PATH_EXTRA < DIR_PATH_MAX);

    NEW(dir);

#ifdef _WIN32
    {
        WIN32_FIND_DATA f;
        char * u8_path = Str_acat(path, "\\*");
        uint16_t* u16_path = u8_to_u16(u8_path);
        dir->_d = FindFirstFileW((LPCWSTR)u16_path, &f);
        dir->first_name = u16_to_u8((uint16_t*)f.cFileName);
        FREE(u8_path);
        FREE(u16_path);
    }
    if (dir->_d == INVALID_HANDLE_VALUE)
#else
    dir->_d = opendir(path);
    if (dir->_d == NULL)
#endif
    {
        Dir_close(dir);
        RAISE_DATA_PTR(Dir_entry_error, path);
    }

    return dir;
}

void Dir_close(T dir)
{
    assert (dir != NULL);

#ifdef _WIN32

    if (dir->_d != INVALID_HANDLE_VALUE)
        FindClose(dir->_d);
#else
    if (dir->_d)
        closedir(dir->_d);
#endif
    FREE(dir);
}

char* Dir_readfile(T dir)
{
    char* filename;

    assert(dir != NULL);

    if(dir->first_name[0] != '\0') {
        char* tmp = dir->first_name;
        dir->first_name[0] = '\0';
        return tmp;
    }

#ifdef _WIN32
    if (FindNextFile(dir->_d, &dir->_f) == 0)
#else
    dir->_e = readdir(dir->_d);
    if (dir->_e == NULL)
#endif
    {
        dir->has_next = 0;
    }

#ifdef _WIN32
    if (GetLastError() != ERROR_SUCCESS &&
        GetLastError() != ERROR_NO_MORE_FILES)
    {
        Dir_close(dir);
        RAISE_INT(Dir_file_error);
    }
#endif


    if(!dir->has_next)
        return NULL;

#ifdef _WIN32
    if (dir->_h == INVALID_HANDLE_VALUE)
        return NULL;
#else
    if (dir->_e != NULL)
        return NULL;
#endif

#ifdef _WIN32
    filename = (char*)u16_to_u8((const uint16_t*) dir->_f.cFileName);
#endif

    if (strlen(dir->path) +
        strlen(
#ifdef _WIN32
            filename
#else
            dir->_e->d_name
#endif
        ) + 1 + _DIR_PATH_EXTRA >=
        _DIR_PATH_MAX)
    {
        RAISE_DATA_PTR(Dir_file_name_too_long, filename);
    }
    if (strlen(
#ifdef _WIN32
            filename
#else
            dir->_e->d_name
#endif
        ) >= _DIR_FILENAME_MAX)
    {
        RAISE_DATA_PTR(Dir_file_name_too_long, filename);
    }

    NEW(file);
    strcpy(file->path, dir->path);
    strcat(file->path, "/");
    strcpy(file->name,
#ifdef _WIN32
        filename
#else
        dir->_e->d_name
#endif
    );

    strcat(file->path, file->name);

    if (stat64(file->path, &file->_s) == -1)
        RAISE_PTR(Dir_file_error);

    file->is_dir =
#ifdef _WIN32
        !!(dir->_f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#else
        S_ISDIR(file->_s.st_mode);
#endif
    file->is_reg =
#ifdef _WIN32
        !!(dir->_f.dwFileAttributes & FILE_ATTRIBUTE_NORMAL);
#else
        S_ISREG(file->_s.st_mode);
#endif

#ifdef _WIN32
    FREE(filename);
#endif
    return file;
}

/*
int dir_readfile_n(const dir_entry *dir, dir_file *file, unsigned i)
{
    if (dir == NULL || file == NULL || i < 0)
    {
        errno = EINVAL;
        return -1;
    }
    if (i >= dir->n_files)
    {
        errno = ENOENT;
        return -1;
    }

    memcpy(file, &dir->_files[i], sizeof(dir_file));

    return 0;
}

dir_entry* Dir_open_subdir_n(dir_entry *dir, unsigned i)
{
    assert(dir);
    assert (i >= dir->n_files || !dir->_files[i].is_dir);

    return Dir_open_sorted(dir->_files[i].path);
}
*/