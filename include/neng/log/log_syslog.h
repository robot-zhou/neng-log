#ifndef __NENG_LOG_SYSLOG_H__
#define __NENG_LOG_SYSLOG_H__

#include <syslog.h>
#include "log.h"

__BEGIN_DECLS

/**
 * @brief   定义了符合条件的系统日志， 输出到指定的facility； 此时日志已经被传递到此syslog appender
 *          mod, tag, level掩码位使用log.h文件定义的NengLogFilterXXXX 函数操作
 */
typedef struct stNengLogSyslogFilter
{
    NengLogFilter filter;
    int facility;
} NengLogSyslogFilter;

/**
 * @brief 系统日志appender结构定义
 * 
 */
typedef struct stNengLogSyslogAppender
{
    NengLogAppender appender; // Nenglog通用Appender定义，请看log.h文件
    char ident[64];           // 系统日志ident， 请看syslog.h定义
    int option;               // 系统日志options， 请看syslog.h定义
    int facility;             // 系统日志facility， 请看syslog.h定义
} NengLogSyslogAppender;

/**
 * @brief 创建syslog类型的appender
 * 
 * @param ident     同opensys函数定义
 * @param option    同opensys函数定义
 * @param facility  同opensys函数定义
 * @return NengLogSyslogAppender* 成功返回非NULL，失败返回NULL
 */
NengLogSyslogAppender *NengLogCreateSyslogAppender(const char *ident, int option, int facility);

// 
// 
/**
 * @brief   给syslog类型的Appender添加系统日志过滤器，注意：这个过滤决定了什么样的日志使用什么facility输出到系统日子；
 *          与NengLogAppender中的filter不同；
 * 
 * @param sys_appender 系统日志appender，
 * @param filter 系统日志内部filter定义；
 * @see     NengLogSyslogAppender
 */
void NengLogSyslogAppenderAddFilter(NengLogSyslogAppender *sys_appender, const NengLogSyslogFilter *filter);

/**
 * @brief 清空Syslog类型的Appender系统日志过滤器
 * 
 * @param sys_appender 系统日志appender
 */
void NengLogSyslogAppenderClearFilter(NengLogSyslogAppender *sys_appender);

__END_DECLS

#endif
