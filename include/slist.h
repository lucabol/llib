#ifndef SLIST_INCLUDED
#define SLIST_INCLUDED

#define T SList_T
typedef struct T *T;

struct T {
	T rest;
	void *first;
};

extern T      SList_new         ();
extern T      SList_append      (T list, T tail);
extern T      SList_copy        (T list);
extern T      SList_list        (void *x, ...);
extern T      sList_pop_front   (T list, void **x);
extern T      SList_push_front  (T list, void *x);
extern T      SList_reverse     (T list);
extern int    SList_length      (T list);
extern void   SList_free        (T *list);
extern void   SList_map         (T list, void apply(void **x, void *cl), void *cl);
extern void **SList_toArray     (T list, void *end);

#undef T
#endif
