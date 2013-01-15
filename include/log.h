#ifndef LOG_INCLUDED
#define LOG_INCLUDED

#include <stdio.h>

#include "utils.h"

BEGIN_DECLS

#define LOG_DISABLE  (0)
#define LOG_FATAL    (1)
#define LOG_ERR      (2)
#define LOG_WARN     (3)
#define LOG_INFO     (4)
#define LOG_DBG      (5)

extern void get_time(char*);

#define LOG_LEVEL(level, ...)   STMT_START {\
                                    if (level <= debug_level) { \
                                        char buf[80]; get_time(buf); \
                                        fprintf(dbgstream, buf); \
                                        fprintf(dbgstream," %-10.20s:%-5.d    ", __FILE__, __LINE__); \
                                        fprintf(dbgstream, __VA_ARGS__); \
                                        fprintf(dbgstream, "\n"); \
                                        fflush(dbgstream); \
                                    } \
                                } STMT_END

#define log(...)        LOG_LEVEL(LOG_INFO, __VA_ARGS__)
#define log_fatal(...)  LOG_LEVEL(LOG_FATAL, __VA_ARGS__)
#define log_err(...)    LOG_LEVEL(LOG_ERROR, __VA_ARGS__)
#define log_warn(...)   LOG_LEVEL(LOG_WARN, __VA_ARGS__)
#define log_info(...)   LOG_LEVEL(LOG_INFO, __VA_ARGS__)
#define log_dbg(...)    LOG_LEVEL(LOG_DBG, __VA_ARGS__)

extern FILE *dbgstream;
extern int  debug_level;

/* If FILE is NULL, set it to stderr, if level is NULL, set it to 1 */

extern void log_init(FILE* where, int level);

END_DECLS

#endif
