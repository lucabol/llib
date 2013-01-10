#include <stdlib.h> /* for malloc */
#include <string.h> /* for strcpy */

#include "assert.h"
#include "test.h"

/* Use the list module */

struct test_node {
    struct test_node* next;
    char* library;
    char* feature;
    test_func func;
} *head = NULL, *tail = NULL;

static
char* test_strdup (const char *s) {
    char *d = malloc (strlen (s) + 1);
    assert(d);

    strcpy (d,s);
    return d; 
}

void test_add(char* library, char* feature, test_func f) {
    struct test_node* p = NULL;

    assert(library);
    assert(feature);
    assert(f);

    p           = malloc(sizeof(*p)); /* use mem */
    assert(p);

    p->func     = f;
    p->library  = test_strdup(library);
    p->feature  = test_strdup(feature);
    p->next     = NULL;

    if(!head) {
        head        = tail = p;
    } else {
        tail->next = p;
        tail        = p;
    }
}

int test_run_all() {
    struct test_node* p;
    int code = 0;

    /* Execute tests*/
    for(p = head; p; p = p->next) {
        int status = 0;
        
        status = p->func();
        printf("/%s/%s/ %s\n", p->library, p->feature, status == TEST_SUCCESS ? "Ok" : "Failed!!!");
        /* A positive value is failure code supposed to be returned from main */
        if(status == TEST_FAILURE) code = 3; 
    }
    printf("\n");

    /* Free tests */
    while (head) {
        struct test_node* q = head->next;

        free(head->library);
        free(head->feature);
        free(head);

        head = q;
    }

    return code;
}

