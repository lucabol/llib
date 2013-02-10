#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#include "str.h"
#include "file.h"
#include "assert.h"
#include "mem.h"


#ifdef _MSC_VER
/* extra chars for the "\\*" mask */
#define PATH_EXTRA 2
#else
#define PATH_EXTRA 0
#endif

#ifdef _MSC_VER
#define strncasecmp _strnicmp
#endif

#define D dir
#define F fileinfo

const Except_T File_Error = {"Error opening file"};

typedef struct D
{
	char path[PATH_MAX];
	int has_next;
	int n_files;

	F _files;
#ifdef _MSC_VER
	HANDLE _h;
	WIN32_FIND_DATA _f;
#else
	DIR *_d;
	struct dirent *_e;
#endif
};

D dir_open(const char *path)
{
#ifdef _MSC_VER
    LPCWSTR wpath;
#endif
    D dir;
    assert(path);
    assert(*path != '\0');
    assert(strlen(path) + PATH_EXTRA < PATH_MAX);

    NEW(dir);
	

	/* initialise dir */
	dir->_files = NULL;
#ifdef _MSC_VER
	dir->_h = INVALID_HANDLE_VALUE;
#else
	dir->_d = NULL;
#endif
	dir_close(dir);

	strcpy(dir->path, path);
#ifdef _MSC_VER
	strcat(dir->path, "\\*");
    wpath = (LPCWSTR) Str_atoucs(dir->path);
	dir->_h = FindFirstFile(wpath, &dir->_f);
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

	return dir;

bail:
	dir_close(dir);
    RAISE(File_Error);
	return NULL;
}

int file_cmp(const void *a, const void *b)
{
	F fa = (F) a;
	F fb = (F) b;
	if (fa->is_dir != fb->is_dir)
	{
		return -(fa->is_dir - fb->is_dir);
	}
	return strncasecmp(fa->name, fb->name, FILENAME_MAX);
}

int dir_open_sorted(D dir, const char *path)
{
	if (dir_open(dir, path) == -1)
	{
		return -1;
	}

	dir->n_files = 0;
	while (dir->has_next)
	{
		F p_file;
		dir->n_files++;
		dir->_files = (F)realloc(dir->_files, sizeof(struct F)*dir->n_files);
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

	qsort(dir->_files, dir->n_files, sizeof(F), file_cmp);

	return 0;

bail:
	dir_close(dir);
	return -1;
}

void dir_close(D dir)
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
    FREE(dir);
}

int dir_next(D dir)
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
		dir_close(dir);
		errno = EIO;
		return -1;
	}
#endif

	return 0;
}

F dir_readfile(D dir)
{
    F file;
    assert(dir != NULL);

#ifdef _MSC_VER
    assert(dir->_h != INVALID_HANDLE_VALUE);
#else
	assert(dir->_e != NULL);
#endif
	assert (strlen(dir->path) +
		strlen(
#ifdef _MSC_VER
		    (const char*) dir->_f.cFileName
#else
			dir->_e->d_name
#endif
		) + 1 + PATH_EXTRA <
		PATH_MAX);

    assert (strlen(
#ifdef _MSC_VER
			(const char*) dir->_f.cFileName
#else
			dir->_e->d_name
#endif
		) < FILENAME_MAX);

    NEW(file);
	strcpy(file->path, dir->path);
	strcat(file->path, "/");
	strcpy(file->name,
#ifdef _MSC_VER
		(const char*) dir->_f.cFileName
#else
		dir->_e->d_name
#endif
	);
	strcat(file->path, file->name);
#ifndef _MSC_VER
	if (stat(file->path, &file->_s) == -1)
	{
		RAISE(File_Error);
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

	return file;
}

int dir_readfile_n(D dir, F file, int i)
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

	memcpy(file, &dir->_files[i], sizeof(F));

	return 0;
}

int dir_open_subdir_n(D dir, int i)
{
	char path[PATH_MAX];
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
