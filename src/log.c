#include "assert.h"
#include "log.h"

FILE *dbgstream     = NULL;
int  debug_level    = LOG_DISABLE;

void log_init(FILE* where, int level) {
    if(!where) where = stderr;
    if(!level) level = 1;

    dbgstream   = where;
    debug_level = level;
}
