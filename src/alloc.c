#include "gc_internal.h"

void *gc_alloc(size_t size)
{
    pthread_mutex_lock(&gc_lock);
    uintptr_t ptr;
    if (!(ptr = (uintptr_t) malloc(size))){
        pthread_mutex_unlock(&gc_lock);
        return NULL;
    }
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
    {
        pthread_mutex_unlock(&gc_lock);
        gc_run();
        pthread_mutex_lock(&gc_lock);
    }
    pthread_mutex_unlock(&gc_lock);
    return (void *) ptr;
}

void gc_add_global(size_t size, void *ptr)
{
    if(!ptr) return;
    pthread_mutex_lock(&gc_lock);
    gc_ptr_t p = (gc_ptr_t){
        .start = (uintptr_t)ptr,
        .size = size,
        .marked = true,
    };

    gc_list_add(&__gc_object.globals, p);
    pthread_mutex_unlock(&gc_lock);

}

void gc_free(void *ptr)
{
    pthread_mutex_lock(&gc_lock);
    gc_list_t *lst = __gc_object.ptr_map[HASH(ptr) % PTR_MAP_SIZE];
    if (lst && gc_list_exist(lst, (uintptr_t) lst)) {
        gc_list_del(&lst, (uintptr_t) lst);
        gc_mfree(lst);
    }
    pthread_mutex_unlock(&gc_lock);
}
