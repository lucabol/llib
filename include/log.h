#ifndef LOG_INCLUDED
#define LOG_INCLUDED

#include <stdio.h>

#include "utils.h"

#define LOG_DISABLE  (0)
#define LOG_FATAL    (1)
#define LOG_ERR      (2)
#define LOG_WARN     (3)
#define LOG_INFO     (4)
#define LOG_DBG      (5)

#define LOG_LEVEL(level, ...)   STMT_START {\
                                    if (level <= debug_level) { \
                                        fprintf(dbgstream,"%s:%d:", __FILE__, __LINE__); \
                                        fprintf(dbgstream, __VA_ARGS__); \
                                        fprintf(dbgstream, "\n"); \
                                        fflush(dbgstream); \
                                    } \
                                } STMT_END

#define log(...) LOG_LEVEL(LOG_INFO, __VA_ARGS__)

extern FILE *dbgstream;
extern int  debug_level;

/* If FILE is NULL, set it to stderr, if level is NULL, set it to 1 */

extern void log_init(FILE* where, int level);

#endif