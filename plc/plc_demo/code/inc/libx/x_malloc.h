#ifndef __X_MALLOC_H__
#define __X_MALLOC_H__

/**
 * @brief            内存申请器
 * @param[in]  size  申请内存空间大小
 * @return           申请到的内存指针
 */
typedef void * (*f_xmalloc)(size_t size,void * ctx);

/**
 * @brief            内存释放器
 * @param[in]  p     待释放内存指针
 */
typedef void (*f_xfree)(void * p,void * ctx);

typedef struct {
    f_xmalloc malloc;
    f_xfree   free;
    void    * ctx;
}t_xallocator;

void * x_malloc(size_t size);
void * x_zalloc(size_t size);
void   x_free(void * p);

void * x_local_malloc(size_t size);
void * x_local_zalloc(size_t size);
void   x_local_free(void * p);

void x_set_allocator(t_xallocator * allocator);
void x_set_local_allocator(t_xallocator * allocator);

#endif
