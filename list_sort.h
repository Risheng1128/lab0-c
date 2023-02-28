#pragma once

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

struct list_head;

__attribute__((nonnull(2, 3))) typedef int (*list_cmp_func_t)(
    void *,
    const struct list_head *,
    const struct list_head *);

__attribute__((nonnull(2, 3))) void list_sort(void *priv,
                                              struct list_head *head,
                                              list_cmp_func_t cmp);

__attribute__((nonnull(2, 3))) int list_cmp(void *priv,
                                            const struct list_head *list1,
                                            const struct list_head *list2);
