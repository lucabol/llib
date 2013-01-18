#include <string.h>

#include <test.h>
#include <mem.h>
#include <timer.h>
#include <log.h>
#include <slist.h>

typedef struct Person {
    char* name;
    char* region;
    long age;
} Person;

Person* new_person(int base, int nameL, int regionL) {
    Person* p;

    NEW(p);
    p->name     = ALLOC(base + nameL);
    p->region   = ALLOC(base + regionL);

    p->age = 35;
    return p;
}

void free_person(Person* p) {
    FREE(p->name);
    FREE(p->region);
    FREE(p);
}

void test_mem_perf_loop() {
#define loop 100
    int i, base;
    Person* array[loop];

    for(base = 0; base < loop; base++) {
        for(i = 0; i < loop; i++) {
            array[i] = new_person(base, i+1, (i+1) * 2);
        }

        for(i = 0; i < loop; i++) {
            free_person(array[i]);
        }
    }

}

void free_person_wrap(void** p, void* cl) {
    (void)cl;
    free_person((Person*)*p);
}

int test_list() {
#define loop 100
    int i, base;

    for(base = 0; base < loop; base++) {
        
        SList_T l = NULL;
        for(i = 0; i < loop; i++) {
            l = SList_push_front(l, new_person(base, i+1, (i+1) * 2));
        }

        SList_map(l, free_person_wrap, NULL);
        SList_free(&l);
    }
    return TEST_SUCCESS;
}

int test_list_perf() {
    double res = test_perf(test_list);
    Arena_T arena;

    log_set(NULL, LOG_DBG);
    log("List : %10.0f", res);
    log_set(NULL, LOG_DISABLE);

    arena = Arena_new();
    /* Arena_config(10, 10 * 1024); */
    Mem_set_arena(arena);
    res = test_perf(test_list);
    Arena_dispose(arena);
    Mem_set_default();

    log_set(NULL, LOG_DBG);
    log("ListA: %10.0f", res);
    log_set(NULL, LOG_DISABLE);


    return TEST_SUCCESS;
}

int test_mem_perf() {
    Timer_T t;
    double memTime, arenaTime, alignTime;
    Arena_T arena;

    /* standard */
    t = Timer_new_start();

    test_mem_perf_loop();

    memTime = Timer_elapsed_micro_dispose(t);

    /* arena */
    t = Timer_new_start();

    arena = Arena_new();
    /* Arena_config(10, 10 * 1024); */
    Mem_set_arena(arena);

    test_mem_perf_loop();

    Arena_dispose(arena);
    Mem_set_default();
    arenaTime = Timer_elapsed_micro_dispose(t);

    /* aligned */
    t = Timer_new_start();

    Mem_set_align();

    test_mem_perf_loop();

    Mem_set_default();
    alignTime = Timer_elapsed_micro_dispose(t);
    
    log_set(NULL, LOG_DBG);
    log("Mem  : %10.0f", memTime);
    log("Arena: %10.0f", arenaTime);
    log("Align: %10.0f", alignTime);
    log("Ratio: %10.0f", memTime / arenaTime);
    log("Ratio: %10.0f", alignTime / arenaTime);
    log_set(NULL, LOG_DISABLE);
   

#ifdef NDEBUG
    test_assert(arenaTime < memTime * 10);
#endif

    return TEST_SUCCESS;
}
