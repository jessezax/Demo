#ifndef __X_PER_THREAD_H_
#define __X_PER_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

/**
 * Macro to define a per lcore variable "var" of type "type", don't
 * use keywords like "static" or "volatile" in type, just prefix the
 * whole macro.
 */
#define DEF_PER_THREAD(type, name)          \
    __thread __typeof__(type) per_thread_##name

/**
 * Macro to declare an extern per lcore variable "var" of type "type"
 */
#define RTE_DECLARE_PER_LCORE(type, name)           \
    extern __thread __typeof__(type) per_lcore_##name

/**
 * Read/write the per-lcore variable value
 */
#define RTE_PER_LCORE(name) (per_lcore_##name)

#ifdef __cplusplus
}
#endif

#endif /* _RTE_PER_LCORE_H_ */

