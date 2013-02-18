#ifndef PORTABLEDIR_INCLUDED
#define PORTABLEDIR_INCLUDED

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "portable.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32
#define _DIR_PATH_EXTRA 2
#define _DIR_PATH_MAX 256
#define _DIR_FILENAME_MAX 256
#else
#define _DIR_PATH_EXTRA 0
#define _DIR_PATH_MAX 4096
#define _DIR_FILENAME_MAX 256
#endif

typedef struct
{
	char path[_DIR_PATH_MAX];
	char name[_DIR_FILENAME_MAX];
	int is_dir;
	int is_reg;

#ifdef _WIN32
    struct _stat _s;
#else
	struct stat _s;
#endif
} dir_file;

typedef struct
{
	char path[_DIR_PATH_MAX];
	int has_next;
	int n_files;

	dir_file *_files;

#ifdef _WIN32
	HANDLE _h;
	WIN32_FIND_DATA _f;
#else
	DIR *_d;
	struct dirent *_e;
#endif
} dir_dir;

#endif