#include <stdarg.h>
#include <stddef.h>

#include "assert.h"
#include "mem.h"
#include "slist.h"
#define T SList_T

T SList_push_front(T list, void *x) {
    T p;

    NEW(p);
    p->first = x;
    p->rest  = list;
    return p;
}

T SList_list(void *x, ...) {
    va_list ap;
    T list, *p = &list;
    
    va_start(ap, x);
    
    for ( ; x; x = va_arg(ap, void *)) {

        NEW(*p);
        (*p)->first = x;
        p = &(*p)->rest;
    }
    
    *p = NULL;
    va_end(ap);
    
    return list;
}

T SList_append(T list, T tail) {
    T *p = &list;
    
    while (*p) p = &(*p)->rest; /* find the end node, inefficient */
    *p = tail;

    return list;
}

T SList_copy(T list) {
    T head, *p = &head;

    for ( ; list; list = list->rest) {
        
        NEW(*p);
        (*p)->first = list->first;
        p = &(*p)->rest;
    }

    *p = NULL;
    return head;
}

T SList_pop(T list, void **x) {
    assert(list);
    assert(x);

    if (list) {
        T head = list->rest;
        
        if (x)
            *x = list->first;

        FREE(list);
        return head;
    } else
        return list;
}

T SList_reverse(T list) {
    T head = NULL, next;

    for ( ; list; list = next) {
        next = list->rest;
        list->rest = head;
        head = list;
    }

    return head;
}

int SList_length(T list) {

    int n;
    for (n = 0; list; list = list->rest)
        n++;

    return n;
}

void SList_free(T *list) {

    T next;
    assert(list);
    
    for ( ; *list; *list = next) {
        next = (*list)->rest;
        FREE(*list);
    }
}

void SList_map(T list, void apply(void **x, void *cl), void *cl) {
    assert(apply);

    for ( ; list; list = list->rest)
        apply(&list->first, cl);
}

void **SList_toArray(T list, void *end) {
    int i, n = SList_length(list);

    void **array = ALLOC((n + 1)*sizeof (*array));
    for (i = 0; i < n; i++) {
        array[i] = list->first;
        list = list->rest;
    }
    array[i] = end;
    return array;
}