#include "file.h"
#include "str.h"
#include "mem.h"

int tinydir_open(tinydir_dir *dir, const char *path)
{
    if (dir == NULL || path == NULL || strlen(path) == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if (strlen(path) + _TINYDIR_PATH_EXTRA >= _TINYDIR_PATH_MAX)
    {
        errno = ENAMETOOLONG;
        return -1;
    }

    /* initialise dir */
    dir->_files = NULL;
#ifdef _MSC_VER
    dir->_h = INVALID_HANDLE_VALUE;
#else
    dir->_d = NULL;
#endif
    tinydir_close(dir);

    strcpy(dir->path, path);
#ifdef _MSC_VER
    strcat(dir->path, "\\*");
    {
        uint32_t* path = Str_atoucs(dir->path);
        dir->_h = FindFirstFile((LPCWSTR) path, &dir->_f);
        FREE(path);
        //dir->_h = FindFirstFile(dir->path, &dir->_f);

    }
    dir->path[strlen(dir->path) - 2] = '\0';
    if (dir->_h == INVALID_HANDLE_VALUE)
#else
    dir->_d = opendir(path);
    if (dir->_d == NULL)
#endif
    {
        errno = ENOENT;
        goto bail;
    }

    /* read first file */
    dir->has_next = 1;
#ifndef _MSC_VER
    dir->_e = readdir(dir->_d);
    if (dir->_e == NULL)
    {
        dir->has_next = 0;
    }
#endif

    return 0;

bail:
    tinydir_close(dir);
    return -1;
}

int tinydir_open_sorted(tinydir_dir *dir, const char *path)
{
    if (tinydir_open(dir, path) == -1)
    {
        return -1;
    }

    dir->n_files = 0;
    while (dir->has_next)
    {
        tinydir_file *p_file;
        dir->n_files++;
        dir->_files = (tinydir_file *)realloc(dir->_files, sizeof(tinydir_file)*dir->n_files);
        if (dir->_files == NULL)
        {
            errno = ENOMEM;
            goto bail;
        }

        p_file = &dir->_files[dir->n_files - 1];
        if (tinydir_readfile(dir, p_file) == -1)
        {
            goto bail;
        }

        tinydir_next(dir);
    }

    qsort(dir->_files, dir->n_files, sizeof(tinydir_file), _tinydir_file_cmp);

    return 0;

bail:
    tinydir_close(dir);
    return -1;
}

void tinydir_close(tinydir_dir *dir)
{
    if (dir == NULL)
    {
        return;
    }

    memset(dir->path, 0, sizeof(dir->path));
    dir->has_next = 0;
    dir->n_files = -1;
    if (dir->_files != NULL)
    {
        free(dir->_files);
    }
    dir->_files = NULL;
#ifdef _MSC_VER
    if (dir->_h != INVALID_HANDLE_VALUE)
    {
        FindClose(dir->_h);
    }
    dir->_h = INVALID_HANDLE_VALUE;
#else
    if (dir->_d)
    {
        closedir(dir->_d);
    }
    dir->_d = NULL;
    dir->_e = NULL;
#endif
}

int tinydir_next(tinydir_dir *dir)
{
    if (dir == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    if (!dir->has_next)
    {
        errno = ENOENT;
        return -1;
    }

#ifdef _MSC_VER
    if (FindNextFile(dir->_h, &dir->_f) == 0)
#else
    dir->_e = readdir(dir->_d);
    if (dir->_e == NULL)
#endif
    {
        dir->has_next = 0;
    }

#ifdef _MSC_VER
    if (GetLastError() != ERROR_SUCCESS &&
        GetLastError() != ERROR_NO_MORE_FILES)
    {
        tinydir_close(dir);
        errno = EIO;
        return -1;
    }
#endif

    return 0;
}

int tinydir_readfile(const tinydir_dir *dir, tinydir_file *file)
{
#ifdef _MSC_VER
    char* filename;
#endif

    if (dir == NULL || file == NULL)
    {
        errno = EINVAL;
        return -1;
    }
#ifdef _MSC_VER
    if (dir->_h == INVALID_HANDLE_VALUE)
#else
    if (dir->_e == NULL)
#endif
    {
        errno = ENOENT;
        return -1;
    }
#ifdef _MSC_VER
    filename = Str_atoutf8((const uint32_t*) dir->_f.cFileName);
#endif
    if (strlen(dir->path) +
        strlen(
#ifdef _MSC_VER
            filename
#else
            dir->_e->d_name
#endif
        ) + 1 + _TINYDIR_PATH_EXTRA >=
        _TINYDIR_PATH_MAX)
    {
        /* the path for the file will be too long */
        errno = ENAMETOOLONG;
        return -1;
    }
    if (strlen(
#ifdef _MSC_VER
            filename
#else
            dir->_e->d_name
#endif
        ) >= _TINYDIR_FILENAME_MAX)
    {
        errno = ENAMETOOLONG;
        return -1;
    }

    strcpy(file->path, dir->path);
    strcat(file->path, "/");
    strcpy(file->name,
#ifdef _MSC_VER
        filename
#else
        dir->_e->d_name
#endif
    );
    strcat(file->path, file->name);
#ifndef _MSC_VER
    if (stat(file->path, &file->_s) == -1)
    {
        return -1;
    }
#endif
    file->is_dir =
#ifdef _MSC_VER
        !!(dir->_f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#else
        S_ISDIR(file->_s.st_mode);
#endif
    file->is_reg =
#ifdef _MSC_VER
        !!(dir->_f.dwFileAttributes & FILE_ATTRIBUTE_NORMAL);
#else
        S_ISREG(file->_s.st_mode);
#endif
#ifdef _MSC_VER
    FREE(filename);
#endif
    return 0;
}

int tinydir_readfile_n(const tinydir_dir *dir, tinydir_file *file, int i)
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

    memcpy(file, &dir->_files[i], sizeof(tinydir_file));

    return 0;
}

int tinydir_open_subdir_n(tinydir_dir *dir, int i)
{
    char path[_TINYDIR_PATH_MAX];
    if (dir == NULL || i < 0)
    {
        errno = EINVAL;
        return -1;
    }
    if (i >= dir->n_files || !dir->_files[i].is_dir)
    {
        errno = ENOENT;
        return -1;
    }

    strcpy(path, dir->_files[i].path);
    tinydir_close(dir);
    if (tinydir_open_sorted(dir, path) == -1)
    {
        return -1;
    }

    return 0;
}

int _tinydir_file_cmp(const void *a, const void *b)
{
    const tinydir_file *fa = (const tinydir_file *)a;
    const tinydir_file *fb = (const tinydir_file *)b;
    if (fa->is_dir != fb->is_dir)
    {
        return -(fa->is_dir - fb->is_dir);
    }
    return strncasecmp(fa->name, fb->name, _TINYDIR_FILENAME_MAX);
}
