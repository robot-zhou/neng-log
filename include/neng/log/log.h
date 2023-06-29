#ifndef __NENG_LOG_H__
#define __NENG_LOG_H__

#include <stdint.h>
#include <stdarg.h>

__BEGIN_DECLS

//////////////////////////////////////////////////////////////////////////////////////////////
// Log 输出函数

// 模块(mod)和标签(tag)有数字表示， 默认有最大值和最小值；
#define NENG_LOG_MOD_MIN 1
#define NENG_LOG_MOD_MAX 255
#define NENG_LOG_MOD_SIZE (NENG_LOG_MOD_MAX - NENG_LOG_MOD_MIN + 1)
#define NENG_LOG_TAG_MIN 1
#define NENG_LOG_TAG_MAX 255
#define NENG_LOG_TAG_SIZE (NENG_LOG_TAG_MAX - NENG_LOG_TAG_MIN + 1)

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

// 启用/禁用异步模式
void NengLogEnableAsync(int enable);

// 设定日志程序名
void NengLogSetProgName(const char *prog);

// 设置/获取日志级别, mod可以单独设置级别；mod=0时，全局级别
void NengLogSetLevel(int level, int mod);
int NengLogGetLevel(int mod);

// 日志级别名称转换
const char *NengLogLevel2Name(int level);
int NengLogName2Level(const char *name);

// 日志输出强制标志，用法: 与级别位或运算, 如：level | NENG_LOG_FLAG_SYNC
// 强制日志同步输出
#define NENG_LOG_FLAG_SYNC 0x0100

// 日志输出, like vsprintf
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

// 日志输出, like sprintf
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

// 关闭日志Appender，使用logrotate备份日志，调用此函数后，重新打开日志输出
void NengLogClose(void);

// 清理日志模块， 程序退出时执行； 默认: on_exit已经安装；
void NengLogClear(void);

//////////////////////////////////////////////////////////////////////////////////////////////
// Log Appender 相关接口

// APPENDER结构的缓冲区大小，改变后需要重新编译整个neng-log库；
#define NENG_LOG_FILEPATH_SIZE 128
#define NENG_LOG_FUNC_SIZE 64
#define NENG_LOG_HOSTNAME_SIZE 32
#define NENG_LOG_PROGNAME_SIZE 32

// Item: appender 内部表述日志的记录，Appender日志输出函数引用的日志记录
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

// 定义日志头输出选项
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

// 日志匹配条件定义
typedef struct stNengLogMatch
{
    uint8_t mod_bits[(NENG_LOG_MOD_MAX + 7) / 8]; // mod 掩码位， 1个bit对应一个mod序号， 全0表示所有mod
    uint8_t tag_bits[(NENG_LOG_TAG_MAX + 7) / 8]; // tag 掩码位， 1个bit对应一个tag序号， 全0表示所有tag
    uint8_t level_bits[1];                        // level 掩码位， 1个bit对应一个level序号， 全0表示所有level
} NengLogMatch;

// 设定本Match启用的Mod，可以设置多个Mod； 如不设置，则所有Mod的日志都会匹配到此Match；
int NengLogMatchSetModBit(NengLogMatch *match, int mod);
int NengLogMatchGetModBit(NengLogMatch *match, int mod);
int NengLogMatchModBitIsEmpty(NengLogMatch *match);
void NengLogMatchClearModBit(NengLogMatch *match);

// 设定本Match启用的Tag，可以设置多个Tag； 如不设置，则所有Tag的日志都会匹配到此Match；
int NengLogMatchSetTagBit(NengLogMatch *match, int tag);
int NengLogMatchGetTagBit(NengLogMatch *match, int tag);
int NengLogMatchTagBitIsEmpty(NengLogMatch *match);
void NengLogMatchClearTagBit(NengLogMatch *match);

// 设定本Match启用的Level，可以设置多个Level； 如不设置，则所有Level的日志都会匹配到此Match；
// 注意： 此处的level与上述设定日志级别不同；此处是掩码位； 如：需要匹配notice，error两个级别的日志，需要调用两次
int NengLogMatchSetLevelBit(NengLogMatch *match, int level);
int NengLogMatchGetLevelBit(NengLogMatch *match, int level);
int NengLogMatchLevelBitIsEmpty(NengLogMatch *match);
void NengLogMatchClearLevelBit(NengLogMatch *match);

struct stNengLogAppender;
// appender 日志输出接口
typedef void (*NengLogItemWriterFunc)(struct stNengLogAppender *appender, const NengLogItem *item);
// appender 释放接口
typedef void (*NengLogAppenderReleaseFunc)(struct stNengLogAppender *appender);
// appender 日志关闭接口，目的：有项目使用logrotate备份日志，调用此函数重新打开日志；
typedef void (*NengLogAppenderCloseFunc)(struct stNengLogAppender *appender);

// Appender 定义
typedef struct stNengLogAppender
{
    char name[16];
    NengLogItemWriterFunc writer_fn;
    NengLogAppenderReleaseFunc release_fn;
    NengLogAppenderCloseFunc close_fn;
    NengLogAppenderFlags flags;
    NengLogMatch match;
} NengLogAppender;

// appender管理，appender添加后， 有neng-log调用release_fn函数变量释放
int NengLogAddAppender(NengLogAppender *appender);
int NengLogRemoveAppender(NengLogAppender *appender);
void NengLogClearAppender(void);

// 通用的日志头生产函数
int NengLogWriteHeader(NengLogAppenderFlags *flags, const NengLogItem *item, char *buf, int bufsiz);

__END_DECLS

#endif