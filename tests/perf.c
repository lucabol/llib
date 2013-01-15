#include <string.h>

#include <test.h>
#include <mem.h>

typedef struct Person {
    char* name;
    char region[40];
    long age;
} Person;

Person* new_person() {
    Person* p;

    NEW(p);
    p->name = ALLOC(20);

    strcpy(p->name, "Luca");
    strcpy(p->region, "Catchacka");
    p->age = 35;
    return p;
}

void free_person(Person* p) {
    FREE(p->name);
    FREE(p);
}

int test_mem_perf() {
    return TEST_SUCCESS;
}