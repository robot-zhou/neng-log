#include <stdio.h>

#include <string.h>
#include <unistd.h>

#include <neng/log/log.h>
#include <neng/log/log_file.h>
#include <neng/log/log_syslog.h>

int main(int argc, char *argv[])
{
    printf("Hello, this is neng-log test program.\n");

    // 设定程序名
    NengLogSetProgName(argv[0]);

    // 设定全局(mod=0)日志级别, 默认: info
    NengLogSetLevel(kNengLogInfo, 0);

    //----------------------------------------------------------------------------------
    // 新建文件日志Appender；test-1.log 只有mod 10， tag 20， notice级别的日志才可以输出；
    NengLogFileAppender *file_appender_1 = NengLogCreateFileAppender("/var/log/test-1.log");

    // 设定文件日志输出选项, 可以不设置，使用默认值;
    file_appender_1->appender.flags.enable_hostname = 1;
    file_appender_1->appender.flags.enable_progname = 1;
    file_appender_1->appender.flags.enable_tid = 1;
    file_appender_1->appender.flags.enable_mod = 1;
    file_appender_1->appender.flags.enable_tag = 1;
    file_appender_1->appender.flags.enable_file = 1;
    file_appender_1->appender.flags.enable_func = 1;
    file_appender_1->appender.flags.enable_fullpath = 0;

    // 设置此文件Appender的日志过滤条件
    {
        NengLogFilter filter = {0};

        // 设置日志过滤参数, 10号模块输出
        NengLogFilterSetModBit(&filter, 10);

        // 设置日志过滤参数, 20号标签输出
        NengLogFilterSetTagBit(&filter, 20);

        // 设置日志过滤参数，Notice日志级别, 仅有Notice级别日志输出；可以调用多次设定多个日志级别
        NengLogFilterSetLevelBit(&filter, kNengLogNotice);

        // 添加Filter
        NengLogAppenderAddFilter(&(file_appender_1->appender), &filter);
    }

    // 添加文件日志Appender
    NengLogAddAppender(&(file_appender_1->appender));

    //----------------------------------------------------------------------------------
    // 新建文件日志Appender；test-2.log日志文件未做mod，tag，level设置
    NengLogFileAppender *file_appender_2 = NengLogCreateFileAppender("/var/log/test-2.log");

    // 设定文件日志输出选项
    // file_appender_2->appender.flags.enable_hostname = 1;
    // file_appender_2->appender.flags.enable_progname = 1;
    // file_appender_2->appender.flags.enable_tid = 1;
    // file_appender_2->appender.flags.enable_mod = 1;
    // file_appender_2->appender.flags.enable_tag = 1;
    // file_appender_2->appender.flags.enable_file = 1;
    // file_appender_2->appender.flags.enable_func = 1;
    // file_appender_2->appender.flags.enable_fullpath = 0;

    // 添加文件日志Appender
    NengLogAddAppender(&(file_appender_2->appender));

    //----------------------------------------------------------------------------------
    // 新建syslog日志appender
    NengLogSyslogAppender *sys_appender = NengLogCreateSyslogAppender(NULL, LOG_ODELAY | LOG_PID, LOG_LOCAL0);

    // 设定系统日志输出选项，
    sys_appender->appender.flags.enable_tid = 1;
    sys_appender->appender.flags.enable_mod = 1;
    sys_appender->appender.flags.enable_tag = 1;
    sys_appender->appender.flags.enable_file = 1;
    sys_appender->appender.flags.enable_func = 1;
    sys_appender->appender.flags.enable_fullpath = 0;

    // 设置系统日志过滤条件
    {
        NengLogFilter filter = {0};

        // 设置日志过滤参数, 10号模块输出
        NengLogFilterSetModBit(&(filter), 10);

        // 设置日志过滤参数, 20号标签输出
        NengLogFilterSetTagBit(&(filter), 20);

        // 设置日志过滤参数，Notice日志级别, 仅有Notice级别日志输出；可以调用多次设定多个日志级别
        NengLogFilterSetLevelBit(&(filter), kNengLogNotice);

        // 添加Filter
        NengLogAppenderAddFilter(&(sys_appender->appender), &filter);
    }

    // 设置制定的mod，tag，级别日志到指定的facility
    {
        NengLogSyslogFilter sys_filter = {0};

        // 指定mod 10
        NengLogFilterSetModBit(&(sys_filter.filter), 10);

        // 指定tag 20
        NengLogFilterSetTagBit(&(sys_filter.filter), 20);

        // 指定level error
        NengLogFilterSetLevelBit(&(sys_filter.filter), kNengLogError);
        NengLogFilterSetLevelBit(&(sys_filter.filter), kNengLogNotice);

        // 指定输出到local6 facility, 不指定local，则输出到NengLogCreateSyslogAppender的参数facility
        sys_filter.facility = LOG_LOCAL6;

        // 设定Filter
        NengLogSyslogAppenderAddFilter(sys_appender, &sys_filter);
    }

    // 添加syslog日志appender。
    NengLogAddAppender(&(sys_appender->appender));

    // 输出日志
    NENG_LOGMT_NOTICE(10, 20, "this is mod-10 tag-20 notice log.");
    NENG_LOGMT_ERROR(10, 25, "this is mod-10 tag-25 error log.");
    NENG_LOG_INFO("this is test info log");
    NENG_LOG_ERROR("this is test error log");
    NENG_LOG_DEBUG("this is test debug log");
    NENG_LOGMT_NOTICE(20, 30, "this is mod-20 tag-30 notice log.");
    NENG_LOGMT_ERROR(20, 35, "this is mod-20 tag-35 error log.");

    return 0;
}