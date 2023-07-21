#ifndef __NENG_LOG_FILE_H__
#define __NENG_LOG_FILE_H__

#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 输出到文件的appender
 *
 */
typedef struct stNengLogFileAppender
{
    NengLogAppender appender;
    char filepath[NENG_LOG_FILEPATH_SIZE]; // 日志文件全路径名，"<stdou>“表示输出标准输出；
    int max_size;                          // 日志文件最大大小，达到最大值，启动循环备份； 0: 不启动循环备份；单位， B，M， G ；
                                           // 日志文件循环备份触发条件：max_size 大于0或者daily等于1
    int max_days;                          // 日志最大保存天数，0: 无日志循环最多天数；
    int max_loop;                          // 日志最多备份数量，如果max_days >0, 此值无效；0: 默认值：30个最大循环备份数量。
    uint16_t daily : 1;                    // 启动每天循环备份
    uint16_t reserve_flags : 15;
} NengLogFileAppender;

/**
 * @brief 新建一个File类型的Appender
 *
 * @param filepath 日志输出路径，"<stdou>“表示输出标准输出；
 * @return NengLogFileAppender* 成功返回非NULL， 失败返回NULL
 */
NengLogFileAppender *NengLogCreateFileAppender(const char *filepath);

#ifdef __cplusplus
}
#endif

#endif
