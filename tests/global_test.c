#include <assert.h>
#include "gc.h"

void *gb1, *gb2, *gb3;

int main(int argc, char *argv[])
{
    gc_init(&argc, 2);

    gc_add_global(sizeof(gb1), (void*)gb1);
    gc_add_global(sizeof(gb2), (void*)gb2);
    gc_add_global(sizeof(gb3), (void*)gb3);

    gb1 = gc_alloc(18);
    gc_dump_internals();

    gb3 = gb1;
    gc_dump_internals();

    gb1 = gc_alloc(24);
    gc_dump_internals();

    gb2 = gc_alloc(20);
    gc_dump_internals();

    gc_destroy();

    return 0;
}
