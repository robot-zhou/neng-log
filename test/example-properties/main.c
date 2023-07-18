#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <neng/log/log.h>
#include <neng/log/log_properties.h>

static int _running = 1;

static void on_signal(int sig)
{
    if (sig == SIGINT)
    {
        _running = 0;
    }
}

int main(int argc, char *argv[])
{
    const char *def_conf = "neng-log.properties";
    const char *conf_file = (argc > 1) ? argv[1] : def_conf;

    // 加载日志配置文件
    NengLogSetProgName(argv[0]);
    if (0 != NengLogLoadProperties(conf_file))
    {
        printf("Error: read properties config file %s failed: %s.\n", conf_file, strerror(errno));
        return -1;
    }

    printf("Hello, this is neng-log test program for read properties config file.\n");
    signal(SIGINT, on_signal);

    // 输出日志
    while (_running)
    {
        sleep(1);

        NENG_LOGMT_NOTICE(10, 20, "this is mod-10 tag-20 notice log.");
        NENG_LOGMT_ERROR(10, 25, "this is mod-10 tag-25 error log.");
        NENG_LOG_INFO("this is test info log");
        NENG_LOG_ERROR("this is test error log");
        NENG_LOG_DEBUG("this is test debug log");
        NENG_LOGMT_NOTICE(20, 30, "this is mod-20 tag-30 notice log.");
        NENG_LOGMT_ERROR(20, 35, "this is mod-20 tag-35 error log.");
    }

    return 0;
}