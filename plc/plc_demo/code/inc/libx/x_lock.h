#ifndef __X_LOCK_H__
#define __X_LOCK_H__

typedef pthread_mutex_t t_mutex;
typedef pthread_spinlock_t t_spinlock;

static inline
void mutex_init(t_mutex * mutex)
{
    //*mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(mutex, NULL);
}

static inline
void mutex_destroy(t_mutex * mutex)
{
    pthread_mutex_destroy(mutex);
}

static inline
int mutex_lock(t_mutex * mutex)
{
    return pthread_mutex_lock(mutex);
}

static inline
int mutex_unlock(t_mutex * mutex)
{
    return pthread_mutex_unlock(mutex);
}

//EBUSY
static inline
int mutex_trylock(t_mutex * mutex)
{
    return pthread_mutex_trylock(mutex);
}


static inline
void spinlock_init(t_spinlock * spinlock)
{
    //*mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_spin_init(spinlock, 0);
}

static inline
void spinlock_destroy(t_spinlock * spinlock)
{
    pthread_spin_destroy(spinlock);
}

static inline
int spinlock_lock(t_spinlock * spinlock)
{
    return pthread_spin_lock(spinlock);
}

static inline
int spinlock_unlock(t_spinlock * spinlock)
{
    return pthread_spin_unlock(spinlock);
}

//EBUSY
static inline
int spinlock_trylock(t_spinlock * spinlock)
{
    return pthread_spin_trylock(spinlock);
}

#endif
