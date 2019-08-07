#ifndef __X_CONF_H__
#define __X_CONF_H__


/**
 * @addtogroup X_CONFIG
 * @{
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "mxml.h"

typedef void * p_conf_ctx;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*! 数据类型     */
typedef enum
{
    XCONF_NONE,      ///< 非数据类型，表示不会把提取的值回写到指定内存地址，只返回配置本身或配置出现的数量
    XCONF_S1,
    XCONF_S2,
    XCONF_S4,
    XCONF_S8,
    XCONF_U1,
    XCONF_U2,
    XCONF_U4,
    XCONF_U8,
    XCONF_DOUBLE,
    XCONF_STRING
} XCONF_VTYPE;

typedef struct {
    t_string file_path;
    mxml_node_t * conf_xml_tree;
}t_conf_ctx;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
   从配置文件获取唯一路径的xml节点属性值。
   @param   dstaddr   目标地址；
   @param   dtype     目标地址空间的数据类型；dtype为字符串时dlen才有意义，最多存放dlen个字符，包括字符串结束符；
   @param   dlen      配置存储的长度（字符串），包含字符串结束符；
   @param   path      指定xml路径（包含属性字符串）的节点的值，此路径在配置文件中唯一出现，不能以'/'开头;
   @return  char*     返回值的字符串(rsn_conf_destroy后指针失效)，失败则返回NULL;
                      如果dstaddr为NULL且dtype为XCONF_NONE，则不会往目标地址写入获取的值，但会返回获取的值
*/
extern const char * x_conf_get_value(p_conf_ctx pctx,void *dstaddr, XCONF_VTYPE dtype, int dlen, const char *path);

/*!
   从配置文件读取指定xml路径的节点数组的数据。
   @param  dstaddr      目标地址；
   @param  dtype        目标地址空间的数据类型；dtype为字符串时dlen才有意义，最多存放dlen个字符，包括字符串结束符；
   @param  dlen         配置存储的长度（字符串），包含字符串结束符；
   @param  list_path    xml路径，path到数组节点，例如<root><list><a id='1'/><a><id value='2'/></a></list></root>，list_path为"root/list/a"
   @param  path         取值路径，path到属性，例如上面的XML字符串，path为"id"或"id/value";
   @param  jump_size    对匹配到多个值，对每个值会跳过定长空间继续填充值；
   @return int          返回匹配到的数量，"<0"-失败返回值；
*/
extern int  x_conf_get_value_arr( p_conf_ctx  pctx,
                                void       *dstaddr,
                                XCONF_VTYPE dtype,
                                int         dlen,
                                const char *list_path,
                                const char *path,
                                int         jump_size);


extern const char *__x_conf_get_value(void *dstaddr, XCONF_VTYPE dtype, int dlen, const char *path, mxml_node_t *node);

p_conf_ctx x_conf_load(const char *conf_path);
void x_conf_free(p_conf_ctx pctx);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif


#endif /* _XCONF_H_ */
