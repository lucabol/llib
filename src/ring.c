#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "assert.h"
#include "ring.h"
#include "mem.h"

#define T Ring_T

struct T {
	struct node {
		struct node *llink, *rlink;
		void *value;
	} *head;
	unsigned length;
};

T Ring_new(void) {
	T ring;

	NEW0(ring);
	ring->head = NULL;

	return ring;
}

T Ring_ring(void *x, ...) {
	va_list ap;

	T ring = Ring_new();

	va_start(ap, x);
	for ( ; x; x = va_arg(ap, void *))
		Ring_push_front(ring, x);
	va_end(ap);

	return ring;
}

void Ring_free(T *ring) {
	struct node *p, *q;
	assert(ring && *ring);

	if ((p = (*ring)->head) != NULL) {
		unsigned n = (*ring)->length;

		for ( ; n-- > 0; p = q) {
			q = p->rlink;
			FREE(p);
		}
	}
	FREE(*ring);
}

int Ring_length(T ring) {
	assert(ring);
	return ring->length;
}

void *Ring_get(T ring, unsigned i) {
	struct node *q;
	assert(ring);
	assert(i < ring->length);

	{
		unsigned n;
		q = ring->head;
		if (i <= ring->length/2)
			for (n = i; n-- > 0; )
				q = q->rlink;
		else
			for (n = ring->length - i; n-- > 0; )
				q = q->llink;
	}

	return q->value;
}

void *Ring_put(T ring, unsigned i, void *x) {
	struct node *q;
	void *prev;
	assert(ring);
	assert(i < ring->length);

	{
		unsigned n;
		q = ring->head;
		if (i <= ring->length/2)
			for (n = i; n-- > 0; )
				q = q->rlink;
		else
			for (n = ring->length - i; n-- > 0; )
				q = q->llink;
	}

	prev = q->value;
	q->value = x;
	return prev;
}

void *Ring_push_front(T ring, void *x) {
	struct node *p, *q;
	assert(ring);

	NEW(p);
	if ((q = ring->head) != NULL)
		{
			p->llink = q->llink;
			q->llink->rlink = p;
			p->rlink = q;
			q->llink = p;
		}
	else
		ring->head = p->llink = p->rlink = p;

	ring->length++;
	return p->value = x;
}

void *Ring_push_back(T ring, void *x) {
	assert(ring);

	Ring_push_front(ring, x);
	ring->head = ring->head->llink;

	return x;
}

void *Ring_add(T ring, signed pos, void *x) {
	assert(ring);
	assert(pos >= -(signed)ring->length && pos<=(signed)ring->length+1);

	if (pos == 1 || pos == -(signed)ring->length)
		return Ring_push_back(ring, x);
	else if (pos == 0 || pos == ring->length + 1)
		return Ring_push_front(ring, x);
	else {
		struct node *p, *q;
		unsigned i = pos < 0 ? pos + ring->length : pos - 1;
		{
			unsigned n;
			q = ring->head;
			if (i <= ring->length/2)
				for (n = i; n-- > 0; )
					q = q->rlink;
			else
				for (n = ring->length - i; n-- > 0; )
					q = q->llink;
		}
		NEW(p);
		{
			p->llink = q->llink;
			q->llink->rlink = p;
			p->rlink = q;
			q->llink = p;
		}
		ring->length++;
		return p->value = x;
	}
}

void *Ring_remove(T ring, unsigned i) {
	void *x;
	struct node *q;
	assert(ring);
	assert(ring->length > 0);
	assert(i < ring->length);

	{
		unsigned n;
		q = ring->head;
		if (i <= ring->length/2)
			for (n = i; n-- > 0; )
				q = q->rlink;
		else
			for (n = ring->length - i; n-- > 0; )
				q = q->llink;
	}

	if (i == 0)
		ring->head = ring->head->rlink;

	x = q->value;
	q->llink->rlink = q->rlink;
	q->rlink->llink = q->llink;
	FREE(q);
	if (--ring->length == 0)
		ring->head = NULL;
	return x;
}

void *Ring_pop_front(T ring) {
	void *x;
	struct node *q;
	assert(ring);
	assert(ring->length > 0);

	q = ring->head->llink;
	x = q->value;
	q->llink->rlink = q->rlink;
	q->rlink->llink = q->llink;
	FREE(q);

	if (--ring->length == 0)
		ring->head = NULL;

	return x;
}

void *Ring_pop_back(T ring) {
	assert(ring);
	assert(ring->length > 0);

	ring->head = ring->head->rlink;
	return Ring_pop_front(ring);
}

void Ring_rotate(T ring, int n) {
	struct node *q;
	int i;
	assert(ring);
	assert(n >= -(signed)ring->length && n <= (signed)ring->length);

	if (n >= 0)
		i = n%ring->length;
	else
		i = n + ring->length;
	{
		unsigned n;
		q = ring->head;
		if (i <= (signed)ring->length/2)
			for (n = i; n-- > 0; )
				q = q->rlink;
		else
			for (n = ring->length - i; n-- > 0; )
				q = q->llink;
	}
	ring->head = q;
}

void Ring_map(T ring, void apply(void **x, void *cl), void *cl) {
	struct node *p, *q;
	assert(ring && apply);

	if ((p = ring->head) != NULL) {
		unsigned n = ring->length;

		for ( ; n-- > 0; p = q) {
			q = p->rlink;
			apply(&p->value, cl);
		}
	}
}

