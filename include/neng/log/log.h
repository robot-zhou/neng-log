/**
 * @file log.h
 * @author robot(robot.zhou@qq.com)
 * @brief neng-log日志模块头文件
 * @version 1.0
 * @date 2023-07-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __NENG_LOG_H__
#define __NENG_LOG_H__

#include <stdint.h>
#include <stdarg.h>

__BEGIN_DECLS

//////////////////////////////////////////////////////////////////////////////////////////////
// 日志输出 相关接口

/**
 * @brief 模块(mod)和标签(tag)有数字表示， 默认有最大值和最小值；
 *
 */
#define NENG_LOG_MOD_SIZE 256
#define NENG_LOG_TAG_SIZE 256

/**
 * @brief 日志级别
 *
 */
enum
{
    kNengLogMinLevel = 0,
    kNengLogEmerg = 0,
    kNengLogAlert = 1,
    kNengLogCrit = 2,
    kNengLogError = 3,
    kNengLogWarn = 4,
    kNengLogNotice = 5,
    kNengLogInfo = 6,
    kNengLogDebug = 7,
    kNengLogMaxLevel = 7,
};

/**
 * @brief 启用/禁用异步模式
 *
 * @param enable, 1: 启用，0: 禁用
 */
void NengLogEnableAsync(int enable);

/**
 * @brief 设定日志输出程序名
 *
 * @param prog ： 程序名，可以输入main的argv[0]作为参数
 */
void NengLogSetProgName(const char *prog);

/**
 * @brief 设置日志级别， 日志级别分全局和模块，如果模块设定了日志级别，模块日志级别有限；
 *
 * @param level： 日志级别，看上述日志级别常量定义；
 * @param mod：模块Id，如果mod=0，设定全局日志级别；
 */
void NengLogSetLevel(int level, int mod);

/**
 * @brief 获取日志级别，mod=0时，获取全局日志级别
 *
 * @param mod : 模块ID
 * @return int ：返回模块的日志级别，mod=0时，返回全局日志级别，
 *              -1，模块未设置日志级别
 */
int NengLogGetLevel(int mod);

/**
 * @brief 清理日志级别设置，mod=0时，恢复全局默认日志级别
 *
 * @param mod: 模块ID
 */
void NengLogClearLevel(int mod);

/**
 * @brief 获取日志级别对应的名称
 *
 * @param level : 日志级别
 * @return const char* : 返回日志级别名称
 */
const char *NengLogLevel2Name(int level);

/**
 * @brief 日志级别名称转日志级别值
 *
 * @param name 日志级别名称
 * @return int 返回日志级别，-1，name是无效的日志级别名称
 */
int NengLogName2Level(const char *name);

/**
 * @brief 日志输出强制标志，用法: 与级别位或运算, 如：level | NENG_LOG_FLAG_SYNC
 *
 */
#define NENG_LOG_FLAG_SYNC 0x0100

/**
 * @brief 输出日志，like vsprintf， 一般不直接调用此函数，而是使用NENG_LOGVXXXX宏输出日志；
 *
 * @param mod 模块ID
 * @param tag 标签ID
 * @param file 源码文件名
 * @param func 源码函数名
 * @param line 源码文件行号
 * @param level 日志级别
 * @param fmt 日志格式化字符串
 * @param ap 可变参数
 */
void NengLogV(int mod, int tag, const char *file, const char *func, int line, int level, const char *fmt, va_list ap);

// 日志输出宏
#define NENG_LOGV(level, fmt, ap)                                     \
    do                                                                \
    {                                                                 \
        NengLogV(0, 0, __FILE__, __func__, __LINE__, level, fmt, ap); \
    } while (0)

#define NENG_LOGVM(mod, level, fmt, ap)                                 \
    do                                                                  \
    {                                                                   \
        NengLogV(mod, 0, __FILE__, __func__, __LINE__, level, fmt, ap); \
    } while (0)

#define NENG_LOGVT(tag, level, fmt, ap)                                 \
    do                                                                  \
    {                                                                   \
        NengLogV(0, tag, __FILE__, __func__, __LINE__, level, fmt, ap); \
    } while (0)

#define NENG_LOGVMT(mod, tag, level, fmt, ap)                             \
    do                                                                    \
    {                                                                     \
        NengLogV(mod, tag, __FILE__, __func__, __LINE__, level, fmt, ap); \
    } while (0)

/**
 * @brief 输出日志，like sprintf 一般不直接调用此函数，而是使用NENG_LOGXXXX宏输出日志；
 *
 * @param mod 模块ID
 * @param tag 标签ID
 * @param file 源码文件名
 * @param func 源码函数名
 * @param line 源码文件行号
 * @param level 日志级别
 * @param fmt 日志格式化字符串, 后继是可变输出函数
 */
void NengLog(int mod, int tag, const char *file, const char *func, int line, int level, const char *fmt, ...);

// 日志输出宏
#define NENG_LOG(level, fmt, ...)                                               \
    do                                                                          \
    {                                                                           \
        NengLog(0, 0, __FILE__, __func__, __LINE__, level, fmt, ##__VA_ARGS__); \
    } while (0)

#define NENG_LOGM(mod, level, fmt, ...)                                           \
    do                                                                            \
    {                                                                             \
        NengLog(mod, 0, __FILE__, __func__, __LINE__, level, fmt, ##__VA_ARGS__); \
    } while (0)

#define NENG_LOGT(tag, level, fmt, ...)                                           \
    do                                                                            \
    {                                                                             \
        NengLog(0, tag, __FILE__, __func__, __LINE__, level, fmt, ##__VA_ARGS__); \
    } while (0)

#define NENG_LOGMT(mod, tag, level, fmt, ...)                                       \
    do                                                                              \
    {                                                                               \
        NengLog(mod, tag, __FILE__, __func__, __LINE__, level, fmt, ##__VA_ARGS__); \
    } while (0)

// 含级别的日志输出宏
#define NENG_LOG_EMERG(fmt, ...) NENG_LOG(kNengLogEmerg, fmt, ##__VA_ARGS__)
#define NENG_LOG_ALERT(fmt, ...) NENG_LOG(kNengLogAlert, fmt, ##__VA_ARGS__)
#define NENG_LOG_CRIT(fmt, ...) NENG_LOG(kNengLogCrit, fmt, ##__VA_ARGS__)
#define NENG_LOG_ERROR(fmt, ...) NENG_LOG(kNengLogError, fmt, ##__VA_ARGS__)
#define NENG_LOG_WARN(fmt, ...) NENG_LOG(kNengLogWarn, fmt, ##__VA_ARGS__)
#define NENG_LOG_NOTICE(fmt, ...) NENG_LOG(kNengLogNotice, fmt, ##__VA_ARGS__)
#define NENG_LOG_INFO(fmt, ...) NENG_LOG(kNengLogInfo, fmt, ##__VA_ARGS__)
#define NENG_LOG_DEBUG(fmt, ...) NENG_LOG(kNengLogDebug, fmt, ##__VA_ARGS__)

#define NENG_LOGM_EMERG(mod, fmt, ...) NENG_LOGM(mod, kNengLogEmerg, fmt, ##__VA_ARGS__)
#define NENG_LOGM_ALERT(mod, fmt, ...) NENG_LOGM(mod, kNengLogAlert, fmt, ##__VA_ARGS__)
#define NENG_LOGM_CRIT(mod, fmt, ...) NENG_LOGM(mod, kNengLogCrit, fmt, ##__VA_ARGS__)
#define NENG_LOGM_ERROR(mod, fmt, ...) NENG_LOGM(mod, kNengLogError, fmt, ##__VA_ARGS__)
#define NENG_LOGM_WARN(mod, fmt, ...) NENG_LOGM(mod, kNengLogWarn, fmt, ##__VA_ARGS__)
#define NENG_LOGM_NOTICE(mod, fmt, ...) NENG_LOGM(mod, kNengLogNotice, fmt, ##__VA_ARGS__)
#define NENG_LOGM_INFO(mod, fmt, ...) NENG_LOGM(mod, kNengLogInfo, fmt, ##__VA_ARGS__)
#define NENG_LOGM_DEBUG(mod, fmt, ...) NENG_LOGM(mod, kNengLogDebug, fmt, ##__VA_ARGS__)

#define NENG_LOGT_EMERG(tag, fmt, ...) NENG_LOGT(tag, kNengLogEmerg, fmt, ##__VA_ARGS__)
#define NENG_LOGT_ALERT(tag, fmt, ...) NENG_LOGT(tag, kNengLogAlert, fmt, ##__VA_ARGS__)
#define NENG_LOGT_CRIT(tag, fmt, ...) NENG_LOGT(tag, kNengLogCrit, fmt, ##__VA_ARGS__)
#define NENG_LOGT_ERROR(tag, fmt, ...) NENG_LOGT(tag, kNengLogError, fmt, ##__VA_ARGS__)
#define NENG_LOGT_WARN(tag, fmt, ...) NENG_LOGT(tag, kNengLogWarn, fmt, ##__VA_ARGS__)
#define NENG_LOGT_NOTICE(tag, fmt, ...) NENG_LOGT(tag, kNengLogNotice, fmt, ##__VA_ARGS__)
#define NENG_LOGT_INFO(tag, fmt, ...) NENG_LOGT(tag, kNengLogInfo, fmt, ##__VA_ARGS__)
#define NENG_LOGT_DEBUG(tag, fmt, ...) NENG_LOGT(tag, kNengLogDebug, fmt, ##__VA_ARGS__)

#define NENG_LOGMT_EMERG(mod, tag, fmt, ...) NENG_LOGMT(mod, tag, kNengLogEmerg, fmt, ##__VA_ARGS__)
#define NENG_LOGMT_ALERT(mod, tag, fmt, ...) NENG_LOGMT(mod, tag, kNengLogAlert, fmt, ##__VA_ARGS__)
#define NENG_LOGMT_CRIT(mod, tag, fmt, ...) NENG_LOGMT(mod, tag, kNengLogCrit, fmt, ##__VA_ARGS__)
#define NENG_LOGMT_ERROR(mod, tag, fmt, ...) NENG_LOGMT(mod, tag, kNengLogError, fmt, ##__VA_ARGS__)
#define NENG_LOGMT_WARN(mod, tag, fmt, ...) NENG_LOGMT(mod, tag, kNengLogWarn, fmt, ##__VA_ARGS__)
#define NENG_LOGMT_NOTICE(mod, tag, fmt, ...) NENG_LOGMT(mod, tag, kNengLogNotice, fmt, ##__VA_ARGS__)
#define NENG_LOGMT_INFO(mod, tag, fmt, ...) NENG_LOGMT(mod, tag, kNengLogInfo, fmt, ##__VA_ARGS__)
#define NENG_LOGMT_DEBUG(mod, tag, fmt, ...) NENG_LOGMT(mod, tag, kNengLogDebug, fmt, ##__VA_ARGS__)

/**
 * @brief 关闭日志Appender，使用logrotate备份日志，调用此函数后，重新打开日志输出
 *
 */
void NengLogClose(void);

/**
 * @brief 清理日志模块， 程序退出时执行； 默认: on_exit已经安装；
 *
 */
void NengLogClear(void);

//////////////////////////////////////////////////////////////////////////////////////////////
// Log Item 相关接口

/**
 * @brief   neng-log内部数据结构的缓冲区标准长度宏定义
 *          改变后需要重新编译整个neng-log库；
 *
 */
#define NENG_LOG_FILEPATH_SIZE 128
#define NENG_LOG_FUNC_SIZE 64
#define NENG_LOG_HOSTNAME_SIZE 32
#define NENG_LOG_PROGNAME_SIZE 32

/**
 * @brief Item: appender 内部表述日志的记录，Appender日志输出函数引用的日志记录
 *
 */
typedef struct stNengLogItem
{
    int64_t time;
    int pid;
    int tid;
    int mod;
    int tag;
    int line;
    int level;
    char file[NENG_LOG_FILEPATH_SIZE];
    char func[NENG_LOG_FUNC_SIZE];
    char hostname[NENG_LOG_HOSTNAME_SIZE];
    char progname[NENG_LOG_PROGNAME_SIZE];
    char content[0];
} NengLogItem;

//////////////////////////////////////////////////////////////////////////////////////////////
// 日志输出头标志 相关

/**
 * @brief 定义日志头输出选项
 *
 */
typedef struct stNengLogAppenderFlags
{
    uint16_t disable_time : 1;
    uint16_t enable_hostname : 1;
    uint16_t enable_progname : 1;
    uint16_t disable_pid : 1;
    uint16_t disable_level : 1;
    uint16_t enable_tid : 1;
    uint16_t enable_mod : 1;
    uint16_t enable_tag : 1;
    uint16_t enable_file : 1;
    uint16_t enable_func : 1;
    uint16_t enable_fullpath : 1;
    uint16_t enable_tab_separator : 1;
    uint16_t disable_async : 1;
    uint16_t reserve_flags : 3;
} NengLogAppenderFlags;

/**
 * @brief 通用的日志头生产函数
 *
 * @param flags 日志头输出标志
 * @param item  neng-log内部日志记录
 * @param buf   日志输出缓冲区
 * @param bufsiz    日志缓冲区大小
 * @return int  成功返回 0， 失败返回 -1
 */
int NengLogWriteHeader(NengLogAppenderFlags *flags, const NengLogItem *item, char *buf, int bufsiz);

//////////////////////////////////////////////////////////////////////////////////////////////
// Log Filter 相关接口

/**
 * @brief   定义日志过滤器，filter定义了什么mod，tag，level的日志会被匹配到， 会用在Appender中，
 *          定义有匹配到的日志会输出到appender
 *
 */
typedef struct stNengLogFilter
{
    uint8_t mod_bits[(NENG_LOG_MOD_SIZE + 7) / 8]; // mod 掩码位， 1个bit对应一个mod序号， 全0表示所有mod
    uint8_t tag_bits[(NENG_LOG_TAG_SIZE + 7) / 8]; // tag 掩码位， 1个bit对应一个tag序号， 全0表示所有tag
    uint8_t level_bits[1];                         // level 掩码位， 1个bit对应一个level序号， 全0表示所有level
} NengLogFilter;

/**
 * @brief   设定、获取、判定是否空、清空日志过滤器的模块Mod；
 *          可以设置多个Mod； 如不设置，则所有Mod的日志都会匹配到此Filter；
 *
 * @param filter    日志过滤器，
 * @param mod       模块Id
 * @return int      设定(Set)， 0: 成功， -1: 失败;
 *                  获取(Get)， 0: mod未设定， 1: mod已经设定，-1: 失败（可能参数校验失败）
 *                  判定(IsEmpty), 0: filter不为空，有设定mod， 1: fitler为空，没有设定Mod
 */
int NengLogFilterSetModBit(NengLogFilter *filter, int mod);
int NengLogFilterGetModBit(NengLogFilter *filter, int mod);
int NengLogFilterModBitIsEmpty(NengLogFilter *filter);
void NengLogFilterClearModBit(NengLogFilter *filter);

/**
 * @brief   设定、获取、判定是否空、清空日志过滤器的标签Tag；
 *          可以设置多个Tag； 如不设置，则所有Tag的日志都会匹配到此Filter；
 *
 * @param filter    日志过滤器，
 * @param tag       标签Id
 * @return int      设定(Set)， 0: 成功， -1: 失败;
 *                  获取(Get)， 0: tag未设定， 1: tag已经设定，-1: 失败（可能参数校验失败）
 *                  判定(IsEmpty), 0: filter不为空，有设定tag， 1: fitler为空，没有设定tag
 */
int NengLogFilterSetTagBit(NengLogFilter *filter, int tag);
int NengLogFilterGetTagBit(NengLogFilter *filter, int tag);
int NengLogFilterTagBitIsEmpty(NengLogFilter *filter);
void NengLogFilterClearTagBit(NengLogFilter *filter);

/**
 * @brief   设定、获取、判定是否空、清空日志过滤器的日志级别；
 *          可以设置多个Level； 如不设置，则所有Level的日志都会匹配到此Filter；
 *
 * @param filter    日志过滤器，
 * @param tag       日志级别
 * @return int      设定(Set)， 0: 成功， -1: 失败;
 *                  获取(Get)， 0: level设定， 1: level已经设定，-1: 失败（可能参数校验失败）
 *                  判定(IsEmpty), 0: filter不为空，有设定level， 1: fitler为空，没有设定level
 */
int NengLogFilterSetLevelBit(NengLogFilter *filter, int level);
int NengLogFilterGetLevelBit(NengLogFilter *filter, int level);
int NengLogFilterLevelBitIsEmpty(NengLogFilter *filter);
void NengLogFilterClearLevelBit(NengLogFilter *filter);

/**
 * @brief 清空Filter，Filter不含有任何mod，tag， level
 *
 * @param filter    日志过滤器
 */
void NengLogFilterClear(NengLogFilter *filter);
//
/**
 * @brief 判断Filter是否为空； 空： 不含有任何mod，tag， level
 *
 * @param filter    日志过滤器
 * @return int  1: 为空，
 *              0: 不为空
 */
int NengLogFilterIsEmpty(NengLogFilter *filter);

//////////////////////////////////////////////////////////////////////////////////////////////
// Log Appender 相关接口

/**
 * @brief 日志Appender定义一个日志输出子模块，
 *
 */

struct stNengLogAppender;

/**
 * @brief 日志appender日志输出函数类型定义，有neng-log框架调用
 *
 */
typedef void (*NengLogItemWriterFunc)(struct stNengLogAppender *appender, const NengLogItem *item);

/**
 * @brief 日志appender清理函数类型定义，有neng-log框架在退出时调用
 *
 */
typedef void (*NengLogAppenderReleaseFunc)(struct stNengLogAppender *appender);

/**
 * @brief 日志appender关闭接口，目的：有项目使用logrotate备份日志，neng-log框架调用此函数重新打开日志；
 *
 */
typedef void (*NengLogAppenderCloseFunc)(struct stNengLogAppender *appender);

/**
 * @brief 日志appender结构，定义一个appender，创建后调用NengLogAddAppender函数添加到neng-log框架
 *
 */
typedef struct stNengLogAppender
{
    char name[16];                         // appender 名称，
    NengLogItemWriterFunc writer_fn;       // 日志输出函数
    NengLogAppenderReleaseFunc release_fn; // appender释放函数
    NengLogAppenderCloseFunc close_fn;     // appender关闭函数
    NengLogAppenderFlags flags;            // appender日志头输出选项
    void *extend;                          // neng-log框架使用，初始化保持为空
} NengLogAppender;

/**
 * @brief 向appender添加日志过滤器，只有匹配到过滤器的日志，才会被投递到此appener；如果appender不含有任何Filter，则所有日志都会被投递到此Appender。
 * 
 * @param appender  日志appender
 * @param filter    日志过滤器
 * @return int      成功返回0， 失败返回-1
 */
int NengLogAppenderAddFilter(NengLogAppender *appender, NengLogFilter *filter);

/**
 * @brief 清空appender的所有日志过滤器，appender不再含有任何日志Filter
 * 
 * @param appender 日志appender
 */
void NengLogAppenderClearFilter(NengLogAppender *appender);

/**
 * @brief 向neng-log框架添加appender，appender添加后， 由neng-log调用release_fn函数变量释放
 * 
 * @param appender 日志appender
 * @return int 成功返回0， 失败返回-1
 */
int NengLogAddAppender(NengLogAppender *appender);

/**
 * @brief 从neng-log中移除appender， 由neng-log调用release_fn函数变量释放
 * 
 * @param appender 日志appender
 * @return int 成功返回0， 失败返回-1
 */
int NengLogRemoveAppender(NengLogAppender *appender);

/**
 * @brief 清空所有appender
 * 
 */
void NengLogClearAppender(void);

/**
 * @brief 清理Appender本身extend成员指向的内部空间，一般由neng-log框架调用；
 * 
 * @param appender 日志Appender
 */
void NengLogAppenderClear(NengLogAppender *appender);

__END_DECLS

#endif