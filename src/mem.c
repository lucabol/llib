#include <stdlib.h>
#ifdef NDEBUG /* Need this to be second to avoid a translation empty error in msvc */

#include <stddef.h>
#include "assert.h"
#include "except.h"
#include "mem.h"
#include "log.h"

const Except_T Mem_Failed = { "Allocation Failed" };

void *Mem_alloc(long nbytes, const char *file, int line){
	void *ptr;
	assert(nbytes > 0);

	ptr = malloc(nbytes);
	if (ptr == NULL)
		{
			if (file == NULL)
				RAISE(Mem_Failed);
			else
				Except_raise(&Mem_Failed, file, line);
		}

	return ptr;
}

void *Mem_calloc(long count, long nbytes,
	const char *file, int line) {
	void *ptr;
	assert(count > 0);
	assert(nbytes > 0);

	ptr = calloc(count, nbytes);
	if (ptr == NULL)
		{
			if (file == NULL)
				RAISE(Mem_Failed);
			else
				Except_raise(&Mem_Failed, file, line);
		}

    log_dbg("%p calloc %li bytes", ptr, nbytes);
	return ptr;
}

void Mem_free(void *ptr, const char *file, int line) {
    (void)file, (void)line;
	if (ptr)
		free(ptr);

    log_dbg("%p freed", ptr);
}

void *Mem_realloc(void *ptr, long nbytes, const char *file, int line) {
	assert(ptr);
	assert(nbytes > 0);

	ptr = realloc(ptr, nbytes);
	if (ptr == NULL)
		{
			if (file == NULL)
				RAISE(Mem_Failed);
			else
				Except_raise(&Mem_Failed, file, line);
		}

    log_dbg("%p realloc %li bytes", ptr, nbytes);
	return ptr;
}

void Mem_print_allocated() {}

#endif /* NDEBUG */
