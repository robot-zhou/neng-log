#include <neng/log/log.h>
#include <neng/log/log_file.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include "log_misc.h"
#include "sysfunc.h"

#define NENG_LOG_FILE_MAX_LOOP 30

typedef struct stNengLogFileContext
{
    int fd;
    long timezone_offset;
    int64_t last_mtime;
    uint64_t file_size;
    uint16_t is_tty : 1;
    uint16_t is_rugar : 1;
    uint16_t is_std : 1;
    uint16_t reserve_flags : 13;
} NengLogFileContext;

#define __FILE_APPENDER(x) (NengLogFileAppender *)((char *)(x) - (size_t) & (((NengLogFileAppender *)0)->appender))
#define __FILE_CONTEXT(fa) (NengLogFileContext *)((char *)(fa) + sizeof(NengLogFileAppender))

static void __closefile(NengLogFileAppender *file_appender);
static int __openfile(NengLogFileAppender *file_appender)
{
    NengLogFileContext *file_context = __FILE_CONTEXT(file_appender);

    if (file_context->fd < 0)
    {
        if (file_appender->filepath[0] == '\0' || strcasecmp(file_appender->filepath, "<stdout>") == 0)
        {
            file_context->fd = STDOUT_FILENO;
            file_context->is_std = 1;
        }
        else
        {
            file_context->fd = open(file_appender->filepath, O_APPEND | O_CREAT | O_WRONLY, 0644);
            if (file_context->fd == -1)
            {
                CRIT_LOG("file appender open file fail: %s", strerror(errno));
                return -1;
            }
        }

        file_context->is_tty = (isatty(file_context->fd) == 1) ? 1 : 0;

        struct stat st = {0};

        if (fstat(file_context->fd, &st) == -1)
        {
            CRIT_LOG("file appender get file stat fail: %s", strerror(errno));
            __closefile(file_appender);
            return -1;
        }

        if (S_ISREG(st.st_mode))
        {
            file_context->is_rugar = 1;
            file_context->file_size = st.st_size;
            file_context->last_mtime = st.st_mtime;
        }
    }

    return 0;
}

static void __closefile(NengLogFileAppender *file_appender)
{
    NengLogFileContext *file_context = __FILE_CONTEXT(file_appender);

    if (file_context->fd > 0)
    {
        fsync(file_context->fd);
        if (file_context->fd > 2)
        {
            close(file_context->fd);
        }

        file_context->fd = -1;
        file_context->is_rugar = 0;
        file_context->is_std = 0;
        file_context->is_tty = 0;
        file_context->file_size = -1;
        file_context->last_mtime = -1;
    }
}

static void __rotatefile(NengLogFileAppender *file_appender)
{
    int n = -1;
    const char *filepath = file_appender->filepath;
    char base_name[NENG_LOG_FILEPATH_SIZE] = {0};
    char ext_name[64] = {0};

    for (n = strlen(filepath) - 1; n >= 0 && filepath[n] != '/' && filepath[n] != '.'; n--)
    {
    }

    if (n >= 0 && filepath[n] == '.')
    {
        strncpy(ext_name, filepath + n + 1, sizeof(ext_name) - 1);
        strncpy(base_name, filepath, n);
    }
    else
    {
        strncpy(base_name, filepath, sizeof(base_name) - 1);
    }

    if (file_appender->max_loop <= 0)
    {
        file_appender->max_loop = NENG_LOG_FILE_MAX_LOOP;
    }

    int max_idx = 0;
    char tmp_filepath[NENG_LOG_FILEPATH_SIZE + 128] = {0};
    char tmp0_filepath[NENG_LOG_FILEPATH_SIZE + 128] = {0};

    if (file_appender->max_days > 0)
    {
        int before_time = neng_log_sys_gettime_millisec() / 1000 - file_appender->max_days * 24 * 3600;

        for (max_idx = 1;; max_idx++)
        {
            struct stat st = {0};
            snprintf(tmp_filepath, sizeof(tmp_filepath), "%s.%d.%s", base_name, max_idx, ext_name);

            if (0 != stat(tmp_filepath, &st))
            {
                if (errno == ENOENT)
                {
                    break;
                }

                continue;
            }

            if (st.st_mtime < before_time)
            {
                break;
            }
        }
    }
    else
    {
        for (max_idx = 1; max_idx <= file_appender->max_loop; max_idx++)
        {
            snprintf(tmp_filepath, sizeof(tmp_filepath), "%s.%d.%s", base_name, max_idx, ext_name);

            if (access(tmp_filepath, F_OK) != 0)
            {
                break;
            }
        }

        if (max_idx > file_appender->max_loop)
        {
            max_idx = file_appender->max_loop;
        }
    }

    for (; max_idx > 0; max_idx--)
    {
        snprintf(tmp_filepath, sizeof(tmp_filepath), "%s.%d.%s", base_name, max_idx, ext_name);

        if (max_idx - 1 > 0)
        {
            snprintf(tmp0_filepath, sizeof(tmp0_filepath), "%s.%d.%s", base_name, max_idx - 1, ext_name);
        }
        else
        {
            snprintf(tmp0_filepath, sizeof(tmp0_filepath), "%s", filepath);
        }

        if (rename(tmp0_filepath, tmp_filepath) != 0)
        {
            CRIT_LOG("rename file %s to %s fail:%s", tmp0_filepath, tmp_filepath, strerror(errno));
        }
    }
}

static void NengLogFileWrite(struct stNengLogAppender *appender, const NengLogItem *item)
{
    NengLogFileAppender *file_appender = __FILE_APPENDER(appender);
    NengLogFileContext *file_context = __FILE_CONTEXT(file_appender);

    if (__openfile(file_appender) != 0)
    {
        return;
    }

    if (file_context->is_rugar && file_context->is_std == 0 && file_context->is_tty == 0 && file_context->file_size > 0 &&
        (file_appender->daily == 1 || file_appender->max_size > 0))
    {
        int rotate = 0;
        const long timezone = file_context->timezone_offset;
        const int day_sec_count = 24 * 60 * 60;
        int64_t nowtm = neng_log_sys_gettime_millisec() / 1000;

        if (file_appender->daily == 1 &&
            ((nowtm + timezone) / day_sec_count != (file_context->last_mtime + timezone) / day_sec_count))
        {
            rotate = 1;
        }

        if (file_appender->max_size > 0 && file_context->file_size > file_appender->max_size)
        {
            rotate = 1;
        }

        if (rotate)
        {
            __closefile(file_appender);
            __rotatefile(file_appender);
            if (__openfile(file_appender) != 0)
            {
                return;
            }
        }
    }

    char buf[NENG_LOG_FILEPATH_SIZE + NENG_LOG_FUNC_SIZE + NENG_LOG_PROGNAME_SIZE + NENG_LOG_HOSTNAME_SIZE + 128] = {0};
    int header_len = NengLogWriteHeader(&(appender->flags), item, buf, sizeof(buf));
    buf[header_len++] = ':';
    buf[header_len] = '\0';

#define __WRITE_WRAP(appender, context, buf, siz)             \
    do                                                        \
    {                                                         \
        int ret = write((context)->fd, (buf), (siz));         \
        if (ret < 0)                                          \
        {                                                     \
            CRIT_LOG("write file fail: %s", strerror(errno)); \
            __closefile((appender));                          \
            return;                                           \
        }                                                     \
        (context)->file_size += ret;                          \
    } while (0)

    __WRITE_WRAP(file_appender, file_context, buf, header_len);
    __WRITE_WRAP(file_appender, file_context, item->content, strlen(item->content));
    __WRITE_WRAP(file_appender, file_context, "\n", 1);
    file_context->last_mtime = neng_log_sys_gettime_millisec() / 1000;
}

static void NengLogFileClose(struct stNengLogAppender *appender)
{
    NengLogFileAppender *file_appender = __FILE_APPENDER(appender);

    __closefile(file_appender);
}

static void NengLogFileRelease(struct stNengLogAppender *appender)
{
    NengLogFileAppender *file_appender = __FILE_APPENDER(appender);

    __closefile(file_appender);

    free(file_appender);
}

NengLogFileAppender *NengLogCreateFileAppender(const char *filepath)
{
    NengLogFileAppender *file_appender = (NengLogFileAppender *)calloc(1, sizeof(NengLogFileAppender) + sizeof(NengLogFileContext));

    if (file_appender == NULL)
    {
        CRIT_LOG("alloc file appender fail: %s", strerror(errno));
        return NULL;
    }

    strncpy(file_appender->appender.name, "file", sizeof(file_appender->appender.name) - 1);
    file_appender->appender.writer_fn = NengLogFileWrite;
    file_appender->appender.close_fn = NengLogFileClose;
    file_appender->appender.release_fn = NengLogFileRelease;

    if (filepath != NULL)
    {
        strncpy(file_appender->filepath, filepath, sizeof(file_appender->filepath) - 1);
    }

    file_appender->max_loop = NENG_LOG_FILE_MAX_LOOP;

    NengLogFileContext *file_context = __FILE_CONTEXT(file_appender);

    file_context->fd = -1;
    file_context->last_mtime = -1;
    file_context->file_size = -1;
    file_context->timezone_offset = neng_log_sys_gettimezone();

    return file_appender;
}