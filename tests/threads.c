#include "gc.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *aaa;
void *bbb;

void *foo1(void *param)
{
    int i = 1;

    gc_init(&param, 1);

    gc_add_global(sizeof(aaa), aaa);
    gc_add_global(sizeof(bbb), bbb);

    aaa = gc_alloc(5992);
    gc_dump_internals();
    bbb = gc_alloc(10000);
    gc_dump_internals();
    aaa = NULL;
    gc_dump_internals();

    while(i++)
    {
        sleep(1);
        void *gss = gc_alloc(i);
        gc_dump_internals();

        if(i >= 4)
            break;
    }
    gc_destroy();
    pthread_exit(NULL);
}

void *foo2(void * param)
{
    int i = 1;

    gc_init(&param, 1);
    
    while(i++)
    {
        sleep(1);
        gc_alloc(i);
        gc_dump_internals();

        if(i >= 4)
            break;
    }
    gc_destroy();
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    /*gc_init(&argc, 1);
    
    void *ptr = gc_alloc(20);
    gc_dump_internals();
    ptr = NULL;
    *ptr = gc_alloc(25);
    gc_dump_internals();
    gc_destroy();
*/

    pthread_t t1;
    pthread_t t2;
    pthread_create(&t1, NULL, foo1, NULL);
    pthread_create(&t2, NULL, foo2, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}