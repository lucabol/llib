#ifndef FILE_H
#define FILE_H

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


/* types */

#define _TINYDIR_PATH_MAX 4096
#ifdef _MSC_VER
/* extra chars for the "\\*" mask */
#define _TINYDIR_PATH_EXTRA 2
#else
#define _TINYDIR_PATH_EXTRA 0
#endif
#define _TINYDIR_FILENAME_MAX 256

#ifdef _MSC_VER
#define strncasecmp _strnicmp
#endif

typedef struct
{
	char path[_TINYDIR_PATH_MAX];
	char name[_TINYDIR_FILENAME_MAX];
	int is_dir;
	int is_reg;

#ifdef _MSC_VER
#else
	struct stat _s;
#endif
} tinydir_file;

typedef struct
{
	char path[_TINYDIR_PATH_MAX];
	int has_next;
	int n_files;

	tinydir_file *_files;
#ifdef _MSC_VER
	HANDLE _h;
	WIN32_FIND_DATA _f;
#else
	DIR *_d;
	struct dirent *_e;
#endif
} tinydir_dir;


/* declarations */

int tinydir_open(tinydir_dir *dir, const char *path);
int tinydir_open_sorted(tinydir_dir *dir, const char *path);
void tinydir_close(tinydir_dir *dir);

int tinydir_next(tinydir_dir *dir);
int tinydir_readfile(const tinydir_dir *dir, tinydir_file *file);
int tinydir_readfile_n(const tinydir_dir *dir, tinydir_file *file, int i);
int tinydir_open_subdir_n(tinydir_dir *dir, int i);

int _tinydir_file_cmp(const void *a, const void *b);

#endif
