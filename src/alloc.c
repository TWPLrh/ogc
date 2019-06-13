#include "gc_internal.h"

void *gc_alloc(size_t size)
{
    uintptr_t ptr;
    if (!(ptr = (uintptr_t) malloc(size)))
        return NULL;

    gc_ptr_t p = (gc_ptr_t){
        .start = ptr,
        .size = size,
        .marked = true,
    };
    if (__gc_object.min > ptr)
        __gc_object.min = ptr;
    if (__gc_object.max < ptr + size)
        __gc_object.max = ptr + size;
    gc_list_add(&__gc_object.ptr_map[HASH(ptr) % PTR_MAP_SIZE], p);
    __gc_object.ptr_num++;
    if (__gc_object.ptr_num >= __gc_object.limit)
        gc_run();
    return (void *) ptr;
}

void gc_add_global(size_t size, void *ptr)
{
    if(!ptr) return;

    gc_ptr_t p = (gc_ptr_t){
        .start = (uintptr_t)ptr,
        .size = size,
        .marked = true,
    };

    gc_list_add(&__gc_object.globals, p);

}

void gc_free(void *ptr)
{
    gc_list_t *lst = __gc_object.ptr_map[HASH(ptr) % PTR_MAP_SIZE];
    if (lst && gc_list_exist(lst, (uintptr_t) lst)) {
        gc_list_del(&lst, (uintptr_t) lst);
        gc_mfree(lst);
    }
}
