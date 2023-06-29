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
        // 设置日志过滤参数, 10号模块输出
        NengLogMatchSetModBit(&(file_appender_1->appender.match), 10);

        // 设置日志过滤参数, 20号标签输出
        NengLogMatchSetTagBit(&(file_appender_1->appender.match), 20);

        // 设置日志过滤参数，Notice日志级别, 仅有Notice级别日志输出；可以调用多次设定多个日志级别
        NengLogMatchSetLevelBit(&(file_appender_1->appender.match), kNengLogNotice);
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
        // 设置日志过滤参数, 10号模块输出
        NengLogMatchSetModBit(&(sys_appender->appender.match), 10);

        // 设置日志过滤参数, 20号标签输出
        NengLogMatchSetTagBit(&(sys_appender->appender.match), 20);

        // 设置日志过滤参数，Notice日志级别, 仅有Notice级别日志输出；可以调用多次设定多个日志级别
        NengLogMatchSetLevelBit(&(sys_appender->appender.match), kNengLogNotice);
    }

    // 设置制定的mod，tag，级别日志到指定的facility
    {
        NengLogSyslogMatch sys_match = {0};

        // 指定mod 10
        NengLogMatchSetModBit(&(sys_match.match), 10);

        // 指定tag 20
        NengLogMatchSetTagBit(&(sys_match.match), 20);

        // 指定level error
        NengLogMatchSetLevelBit(&(sys_match.match), kNengLogError);
        NengLogMatchSetLevelBit(&(sys_match.match), kNengLogNotice);

        // 指定输出到local6 facility, 不指定local，则输出到NengLogCreateSyslogAppender的参数facility
        sys_match.facility = LOG_LOCAL6;

        // 设定match
        NengLogSyslogAppenderAddMatch(sys_appender, &sys_match);
    }

    // 添加syslog日志appender。
    NengLogAddAppender(&(sys_appender->appender));

    // 输出日志
    NENG_LOGMT_NOTICE(10, 20, "this is mod-20 tag-10 notice log.");
    NENG_LOG_INFO("this is test info log");
    NENG_LOG_ERROR("this is test error log");

    return 0;
}