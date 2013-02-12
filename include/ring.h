/* $Id: ring.h 6 2007-01-22 00:45:22Z drhanson $ */
#ifndef RING_INCLUDED
#define RING_INCLUDED

#define T Ring_T
typedef struct T *T;

extern T     Ring_new (void);
extern T     Ring_ring(void *x, ...);
extern void  Ring_free  (T *ring);
extern int   Ring_length(T  ring);
extern void *Ring_get(T ring, unsigned i);
extern void *Ring_put(T ring, unsigned i, void *x);
extern void *Ring_add(T ring, signed pos, void *x);
extern void *Ring_push_back(T ring, void *x);
extern void *Ring_push_front(T ring, void *x);
extern void *Ring_remove(T ring, unsigned i);
extern void *Ring_pop_back(T ring);
extern void *Ring_pop_front(T ring);
extern void  Ring_rotate(T ring, signed n);
extern void  Ring_map(T ring, void apply(void **x, void *cl), void *cl);

#undef T
#endif
