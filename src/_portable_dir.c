#include "dir.h"
#include "str.h"
#include "mem.h"
#include "utf8.h"

int dir_open(dir_dir *dir, const char *path)
{
    if (dir == NULL || path == NULL || strlen(path) == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if (strlen(path) + _DIR_PATH_EXTRA >= _DIR_PATH_MAX)
    {
        errno = ENAMETOOLONG;
        return -1;
    }

    /* initialize dir */
    dir->_files = NULL;
#ifdef _WIN32
    dir->_h = INVALID_HANDLE_VALUE;
#else
    dir->_d = NULL;
#endif
    dir_close(dir);

    strcpy(dir->path, path);
#ifdef _WIN32
    strcat(dir->path, "\\*");
    {
        uint16_t* path = u8_to_u16((const char*)dir->path);
        dir->_h = FindFirstFileW((LPCWSTR)path, &dir->_f);
        FREE(path);
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
#ifndef _WIN32
    dir->_e = readdir(dir->_d);
    if (dir->_e == NULL)
    {
        dir->has_next = 0;
    }
#endif

    return 0;

bail:
    dir_close(dir);
    return -1;
}

int _dir_file_cmp(const void *a, const void *b)
{
    const dir_file *fa = (const dir_file *)a;
    const dir_file *fb = (const dir_file *)b;
    if (fa->is_dir != fb->is_dir)
    {
        return -(fa->is_dir - fb->is_dir);
    }
    return strncasecmp(fa->name, fb->name, _DIR_FILENAME_MAX);
}

int dir_open_sorted(dir_dir *dir, const char *path)
{
    if (dir_open(dir, path) == -1)
    {
        return -1;
    }

    dir->n_files = 0;
    while (dir->has_next)
    {
        dir_file *p_file;
        dir->n_files++;
        dir->_files = (dir_file *)realloc(dir->_files, sizeof(dir_file)*dir->n_files);
        if (dir->_files == NULL)
        {
            errno = ENOMEM;
            goto bail;
        }

        p_file = &dir->_files[dir->n_files - 1];
        if (dir_readfile(dir, p_file) == -1)
        {
            goto bail;
        }

        dir_next(dir);
    }

    qsort(dir->_files, dir->n_files, sizeof(dir_file), _dir_file_cmp);

    return 0;

bail:
    dir_close(dir);
    return -1;
}

void dir_close(dir_dir *dir)
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
#ifdef _WIN32
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

int dir_next(dir_dir *dir)
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

#ifdef _WIN32
    if (FindNextFile(dir->_h, &dir->_f) == 0)
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
        dir_close(dir);
        errno = EIO;
        return -1;
    }
#endif

    return 0;
}

int dir_readfile(const dir_dir *dir, dir_file *file)
{
#ifdef _WIN32
    char* filename;
#endif

    if (dir == NULL || file == NULL)
    {
        errno = EINVAL;
        return -1;
    }
#ifdef _WIN32
    if (dir->_h == INVALID_HANDLE_VALUE)
#else
    if (dir->_e == NULL)
#endif
    {
        errno = ENOENT;
        return -1;
    }
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
        /* the path for the file will be too long */
        errno = ENAMETOOLONG;
        return -1;
    }
    if (strlen(
#ifdef _WIN32
            filename
#else
            dir->_e->d_name
#endif
        ) >= _DIR_FILENAME_MAX)
    {
        errno = ENAMETOOLONG;
        return -1;
    }

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
#ifndef _WIN32
    if (stat(file->path, &file->_s) == -1)
    {
        return -1;
    }
#endif
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
    return 0;
}

int dir_readfile_n(const dir_dir *dir, dir_file *file, int i)
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

int dir_open_subdir_n(dir_dir *dir, int i)
{
    char path[_DIR_PATH_MAX];
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
    dir_close(dir);
    if (dir_open_sorted(dir, path) == -1)
    {
        return -1;
    }

    return 0;
}