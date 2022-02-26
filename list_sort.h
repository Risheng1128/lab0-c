#ifndef _LIST_SORT_H
#define _LIST_SORT_H
#include <stdint.h>
#include "list.h"

#define USE_LINUX_SORT 1

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

typedef int
    __attribute__((nonnull(2, 3))) (*list_cmp_func_t)(void *,
                                                      const struct list_head *,
                                                      const struct list_head *);

void list_sort(void *priv, struct list_head *head, list_cmp_func_t cmp);

#endif /* End of _LIST_SORT_H_ */