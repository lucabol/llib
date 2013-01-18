#include <string.h>

#include <test.h>
#include <mem.h>
#include <timer.h>

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
    
    /*
    printf("Mem  : %10.0f\n", memTime);
    printf("Arena: %10.0f\n", arenaTime);
    printf("Align: %10.0f\n", alignTime);
    printf("Ratio: %10.0f\n", memTime / arenaTime);
    printf("Ratio: %10.0f\n", alignTime / arenaTime);
    */

#ifdef NDEBUG
    test_assert(arenaTime < memTime * 10);
#endif

    return TEST_SUCCESS;
}
