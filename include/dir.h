#ifndef FILE_H
#define FILE_H

#include "_portable_dir.h"

int dir_open(dir_dir *dir, const char *path);
int dir_open_sorted(dir_dir *dir, const char *path);
void dir_close(dir_dir *dir);

int dir_next(dir_dir *dir);
int dir_readfile(const dir_dir *dir, dir_file *file);
int dir_readfile_n(const dir_dir *dir, dir_file *file, int i);
int dir_open_subdir_n(dir_dir *dir, int i);

#endif
