#ifndef __X_MODULE_H__
#define __X_MODULE_H__

#define X_MOD_CONSTRUCTOR_PRIO  (1000)
#define X_MOD_NUM (256)

#define __constructor__(prio) __attribute__((constructor(prio)))
#define __destructor__(prio)  __attribute__((destructor(prio)))

#define CTRL_GET (0)
#define CTRL_SET (1)
#define XMOD_CTRL(M,S,E) ((((M)&0xff)<<24) | (((S)&0x01) << 16) | ((E) & 0xffff))

#define XMOD_CTRL_M(C) (((C)>>24) & 0xff)
#define XMOD_CTRL_S(C) (((C)>>16) & 0x01)
#define XMOD_CTRL_E(C) ((C)&0xffff)

typedef struct st_xmodule t_xmodule;

typedef int (*f_xmodule_init)(t_xmodule *);
typedef int (*f_xmodule_uninit)(t_xmodule *);
typedef int (*f_xmodule_reconf)(t_xmodule *);
typedef int (*f_xmodule_ctrl)(t_xmodule *,unsigned int ctrl,void * arg,unsigned int arg_size);

struct st_xmodule {
    const char        *name;
    unsigned char      idx;
    f_xmodule_init     init;
    f_xmodule_uninit   uninit;
    f_xmodule_reconf   reconf;
    f_xmodule_ctrl     ctrl;
    void              *ctx;
};

void __x_module_register(t_xmodule * xmod,unsigned char idx);

#define x_module_register(xmod,idx) \
__constructor__(X_MOD_CONSTRUCTOR_PRIO) static inline void __INIT_REGISTER_##idx(void) \
{ \
    __x_module_register(xmod,idx); \
}

int __module_init(unsigned char idx);
int module_init(void);

int __module_uninit(unsigned char idx);
int module_uninit(void);

int __module_reconf(unsigned char idx);
int module_reconf(void);

int module_ctrl(unsigned int ctrl,void * arg,unsigned int arg_size);

#endif
