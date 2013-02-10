#ifndef TINYDIR_H
#define TINYDIR_H

#include <stdio.h> /* FILENAME_MAX */

#include "except.h"

#define D dir
typedef struct D* D;

#define PATH_MAX 4096

struct fileinfo
{
	char path[PATH_MAX];
	char name[FILENAME_MAX];
	int is_dir;
	int is_reg;

#ifdef _MSC_VER
#else
	struct stat _s;
#endif
};

#define F fileinfo
typedef struct F* F;

extern const Except_T File_Error;

D dir_open(const char *path);
int dir_open_sorted(D dir, const char *path);
void dir_close(D dir);

int dir_next(D dir);
F dir_readfile(D dir);
int dir_readfile_n(D dir, F file, int i);
int dir_open_subdir_n(D dir, int i);

#undef T
#undef F
#endif
