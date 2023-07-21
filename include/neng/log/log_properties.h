#ifndef __NENG_LOG_PROPERTIES_H__
#define __NENG_LOG_PROPERTIES_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 加载properties格式的配置文件；配置例子：neng-log.properties
 * 
 * @param filepath 配置文件路径
 * @return int 成功返回0，失败返回-1
 */
int NengLogLoadProperties(const char *filepath);

#ifdef __cplusplus
}
#endif

#endif