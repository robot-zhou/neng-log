#ifndef __COMMON_ARRAY_H__
#define __COMMON_ARRAY_H__

#include <sys/cdefs.h>

__BEGIN_DECLS

#define ARRAY_HEAD(name, type) \
    struct name                \
    {                          \
        unsigned int size;     \
        unsigned int len;      \
        type *p;               \
    }

#define ARRAY_INIT(x)  \
    do                 \
    {                  \
        (x)->size = 0; \
        (x)->len = 0;  \
        (x)->p = NULL; \
    } while (0)

#define ARRAY_INITIALIZER(x) \
    {                        \
        0, 0, NULL           \
    }

#if 0
#define ARRAY_LOG(fmt, ...) printf("%s:%d " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#endif

#define ARRAY_PROTOTYPE_INTERNAL(name, type, fn_clr, fn_copy, fn_move, fn_cmp, attr)                \
    attr int name##_ARRAY_RESIZE(struct name *x, unsigned int n);                                   \
    attr int name##_ARRAY_INSERT_PTR(struct name *x, int before, const type *pval, unsigned int n); \
    attr int name##_ARRAY_INSERT(struct name *x, int before, const type val);                       \
    attr int name##_ARRAY_APPEND_N(struct name *x, const type val, unsigned n);                     \
    attr int name##_ARRAY_APPEND_PTR(struct name *x, const type *pval, unsigned int n);             \
    attr int name##_ARRAY_APPEND(struct name *x, const type val);                                   \
    attr int name##_ARRAY_REMOVE(struct name *x, unsigned int n, int unsigned len);                 \
    attr int name##_ARRAY_CLEAR(struct name *x);

#define ARRAY_GENERATE_INTERNAL(name, type, fn_clr, fn_copy, fn_move, fn_cmp, attr)                                 \
    attr int name##_ARRAY_RESIZE(struct name *x, unsigned int n)                                                    \
    {                                                                                                               \
        void (*clr_fn)(type * x, int n) = fn_clr;                                                                   \
        if (x->size == n)                                                                                           \
        {                                                                                                           \
            return 0;                                                                                               \
        }                                                                                                           \
        if (x->size > n && x->len > n && clr_fn != NULL)                                                            \
        {                                                                                                           \
            clr_fn(x->p + n, x->len - n);                                                                           \
        }                                                                                                           \
        int nn = (n > 0) ? n + 1 : n;                                                                               \
        type *np = (type *)realloc(x->p, nn * sizeof(type));                                                        \
        if (np == NULL && nn != 0)                                                                                  \
        {                                                                                                           \
            return -1;                                                                                              \
        }                                                                                                           \
        if (x->size < n)                                                                                            \
        {                                                                                                           \
            memset(np + x->size, 0, (n + 1 - x->size) * sizeof(type));                                              \
        }                                                                                                           \
        else if (np != NULL)                                                                                        \
        {                                                                                                           \
            memset(np + n, 0, 1 * sizeof(type));                                                                    \
        }                                                                                                           \
        x->p = np;                                                                                                  \
        x->size = n;                                                                                                \
        x->len = (x->len > n) ? n : x->len;                                                                         \
        return 0;                                                                                                   \
    }                                                                                                               \
    attr int name##_ARRAY_INSERT_PTR(struct name *x, int before, const type *pval, unsigned int n)                  \
    {                                                                                                               \
        void (*copy_fn)(type * x, const type *y, int n) = fn_copy;                                                  \
        void (*move_fn)(type * x, const type *y, int n) = fn_move;                                                  \
        if (pval == NULL || n == 0)                                                                                 \
        {                                                                                                           \
            return 0;                                                                                               \
        }                                                                                                           \
        if (x->size - x->len < n)                                                                                   \
        {                                                                                                           \
            int nsiz = x->size + n;                                                                                 \
            nsiz = (nsiz > 32) ? (nsiz + 63) / 64 * 64 : ((nsiz > 16) ? 32 : (nsiz > 8 ? 16 : 8));                  \
            if (name##_ARRAY_RESIZE(x, nsiz) != 0)                                                                  \
            {                                                                                                       \
                return -1;                                                                                          \
            }                                                                                                       \
        }                                                                                                           \
        before = (before < 0 || before > x->len) ? x->len : before;                                                 \
        if (before < x->len)                                                                                        \
        {                                                                                                           \
            if (move_fn != NULL)                                                                                    \
            {                                                                                                       \
                move_fn(x->p + before + n, x->p + before, x->len - before);                                         \
            }                                                                                                       \
            else                                                                                                    \
            {                                                                                                       \
                memmove(x->p + before + n, x->p + before, (x->len - before) * sizeof(type));                        \
            }                                                                                                       \
        }                                                                                                           \
        if (copy_fn != NULL)                                                                                        \
        {                                                                                                           \
            copy_fn(x->p + before, pval, n);                                                                        \
        }                                                                                                           \
        else                                                                                                        \
        {                                                                                                           \
            memcpy(x->p + before, pval, n * sizeof(type));                                                          \
        }                                                                                                           \
        x->len += n;                                                                                                \
        return 0;                                                                                                   \
    }                                                                                                               \
    attr int name##_ARRAY_INSERT(struct name *x, int before, const type val)                                        \
    {                                                                                                               \
        return name##_ARRAY_INSERT_PTR(x, before, &val, 1);                                                         \
    }                                                                                                               \
    attr int name##_ARRAY_APPEND_PTR(struct name *x, const type *pval, unsigned int n)                              \
    {                                                                                                               \
        return name##_ARRAY_INSERT_PTR(x, -1, pval, n);                                                             \
    }                                                                                                               \
    attr int name##_ARRAY_APPEND(struct name *x, const type val)                                                    \
    {                                                                                                               \
        return name##_ARRAY_APPEND_PTR(x, &val, 1);                                                                 \
    }                                                                                                               \
    attr int name##_ARRAY_APPEND_N(struct name *x, const type val, unsigned n)                                      \
    {                                                                                                               \
        while (n-- > 0)                                                                                             \
        {                                                                                                           \
            if (0 != name##_ARRAY_APPEND_PTR(x, &val, 1))                                                           \
            {                                                                                                       \
                return -1;                                                                                          \
            }                                                                                                       \
        }                                                                                                           \
        return 0;                                                                                                   \
    }                                                                                                               \
    attr int name##_ARRAY_REMOVE(struct name *x, unsigned int n, int unsigned len)                                  \
    {                                                                                                               \
        void (*clr_fn)(type * x, int n) = fn_clr;                                                                   \
        void (*move_fn)(type * x, const type *y, int n) = fn_move;                                                  \
        if (n >= x->len)                                                                                            \
        {                                                                                                           \
            return -1;                                                                                              \
        }                                                                                                           \
        len = (n + len > x->len) ? x->len - n : len;                                                                \
        if (len == 0)                                                                                               \
        {                                                                                                           \
            return 0;                                                                                               \
        }                                                                                                           \
        if (clr_fn != NULL)                                                                                         \
        {                                                                                                           \
            clr_fn(x->p + n, len);                                                                                  \
        }                                                                                                           \
        if (n + len < x->len)                                                                                       \
        {                                                                                                           \
            if (fn_move != NULL)                                                                                    \
            {                                                                                                       \
                move_fn(x->p + n, x->p + n + len, x->len - (n + len));                                              \
            }                                                                                                       \
            else                                                                                                    \
            {                                                                                                       \
                memmove(x->p + n, x->p + n + len, (x->len - (n + len)) * sizeof(type));                             \
            }                                                                                                       \
        }                                                                                                           \
        x->len -= len;                                                                                              \
        memset(x->p + x->len, 0, sizeof(type));                                                                     \
        unsigned int nsiz = (x->len > 32) ? (x->len + 63) / 64 * 64 : ((x->len > 16 ? 32 : (x->len > 8 ? 16 : 8))); \
        name##_ARRAY_RESIZE(x, nsiz);                                                                               \
        return 0;                                                                                                   \
    }                                                                                                               \
    attr int name##_ARRAY_CLEAR(struct name *x)                                                                     \
    {                                                                                                               \
        return name##_ARRAY_RESIZE(x, 0);                                                                           \
    }

#define ARRAY_PROTOTYPE(name, type, fn_clr, fn_copy, fn_move, fn_cmp) \
    ARRAY_PROTOTYPE_INTERNAL(name, type, fn_clr, fn_copy, fn_move, fn_cmp, )
#define ARRAY_GENERATE(name, type, fn_clr, fn_copy, fn_move, fn_cmp) \
    ARRAY_GENERATE_INTERNAL(name, type, fn_clr, fn_copy, fn_move, fn_cmp, )
#define ARRAY_GENERATE_STATIC(name, type, fn_clr, fn_copy, fn_move, fn_cmp) \
    ARRAY_GENERATE_INTERNAL(name, type, fn_clr, fn_copy, fn_move, fn_cmp, static __attribute_maybe_unused__)

#define ARRAY_RESIZE(name, x, size) \
    name##_ARRAY_RESIZE(x, size)
#define ARRAY_INSERT_PTR(name, x, before, pval, n) \
    name##_ARRAY_INSERT_PTR(x, before, pval, n)
#define ARRAY_INSERT(name, x, before, val) \
    name##_ARRAY_INSERT(x, before, val)
#define ARRAY_APPEND_PTR(name, x, pval, n) \
    name##_ARRAY_APPEND_PTR(x, pval, n)
#define ARRAY_APPEND(name, x, val) \
    name##_ARRAY_APPEND(x, val)
#define ARRAY_APPEND_N(name, x, val, n) \
    name##_ARRAY_APPEND_N(x, val, n)
#define ARRAY_REMOVE(name, x, n, len) \
    name##_ARRAY_REMOVE(x, n, len)
#define ARRAY_CLEAR(name, x) \
    name##_ARRAY_CLEAR(x)

__END_DECLS

#endif //__COMMON_ARRAY_H__