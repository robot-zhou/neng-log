#ifndef __NENG_LOG_FILTER_H__
#define __NENG_LOG_FILTER_H__

__BEGIN_DECLS

/**
 * @brief 判断Filter是否为空； 空： 不含有任何mod，tag， level
 *
 * @param filter    日志过滤器
 * @return int  1: 为空，
 *              0: 不为空
 */
int NengLogFilterIsEmpty(NengLogFilter *filter);

/**
 * @brief 坚持日志过滤器是否匹配指定的日志
 *
 * @param filter    日志过滤器
 * @param mod       模块
 * @param tag       标签
 * @param level     级别
 * @return int      0:  匹配失败
 *                  1： 匹配成功
 */
int NengLogFilterHit(NengLogFilter *filter, int mod, int tag, int level);

__END_DECLS

#endif // __NENG_LOG_FILTER_H__