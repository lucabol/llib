#ifndef RAND_INCLUDED
#define RAND_INCLUDED

#include "portable.h" /* for thread_local */
#include "utils.h"

BEGIN_DECLS

extern thread_local int rseed;

inline void Rand_init(int x){	
    rseed = x;
}

// Uses Microsoft algorithm for 
#define RAND_BITS   ((1U << 31) - 1)
#define RAND_MAXO   ((1U << 15) - 1) 
#define RAND_A      214013
#define RAND_B      2531011

inline double Rand_next(){	
    return ((double)((rseed = (rseed * RAND_A + RAND_B) & RAND_BITS) >> 16)) / RAND_MAXO;
}

END_DECLS

#endif
