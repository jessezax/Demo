#include "libx.h"

static inline 
void * __malloc(size_t size,void * ctx)
{
    return malloc(size);
}

static inline 
void __free(void * p,void * ctx)
{
    return free(p);
}

static t_xallocator g_allocator = {
    .malloc = __malloc,
    .free   = __free,
    .ctx    = NULL
};

static t_xallocator g_local_allocator = {
    .malloc = __malloc,
    .free   = __free,
    .ctx    = NULL
};

void x_set_allocator(t_xallocator * allocator)
{
    if (allocator->malloc) {
        g_allocator.malloc = allocator->malloc;
    } else {
        g_allocator.malloc = __malloc;
    }
    if (allocator->free) {
        g_allocator.free = allocator->free;
    } else {
        g_allocator.free = __free;
    }
    if (allocator->ctx) {
        g_allocator.ctx = allocator->ctx;
    } else {
        g_allocator.ctx = NULL;
    }
}

void x_set_local_allocator(t_xallocator * allocator)
{
    if (allocator->malloc) {
        g_local_allocator.malloc = allocator->malloc;
    } else {
        g_local_allocator.malloc = __malloc;
    }
    if (allocator->free) {
        g_local_allocator.free = allocator->free;
    } else {
        g_local_allocator.free = __free;
    }
    if (allocator->ctx) {
        g_local_allocator.ctx = allocator->ctx;
    } else {
        g_local_allocator.ctx = NULL;
    }
}

void * x_malloc(size_t size)
{
    return g_allocator.malloc(size,g_allocator.ctx);
}

void x_free(void * p)
{
    g_allocator.free(p,g_allocator.ctx);
}

void * x_zalloc(size_t size)
{
    void * p = x_malloc(size);
    if (p) {
        memset(p,0x00,size);
    }
    return p;
}

void * x_local_malloc(size_t size)
{
    return g_local_allocator.malloc(size,g_local_allocator.ctx);
}

void x_local_free(void * p)
{
    g_local_allocator.free(p,g_local_allocator.ctx);
}

void * x_local_zalloc(size_t size)
{
    void * p = x_local_malloc(size);
    if (p) {
        memset(p,0x00,size);
    }
    return p;
}

