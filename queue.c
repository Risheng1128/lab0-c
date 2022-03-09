#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "harness.h"
#include "list_sort.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));
    if (!new)
        return NULL;

    INIT_LIST_HEAD(new);
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    element_t *node, *next;
    list_for_each_entry_safe (node, next, l, list) {
        list_del(&node->list);
        q_release_element(node);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    INIT_LIST_HEAD(&new->list);
    int str_size = strlen(s);
    new->value = malloc((str_size + 1) * sizeof(char));

    if (!new->value) {
        free(new);
        return false;
    }

    strncpy(new->value, s, str_size);
    *(new->value + str_size) = '\0';
    list_add(&new->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    INIT_LIST_HEAD(&new->list);
    int str_size = strlen(s);
    new->value = malloc((str_size + 1) * sizeof(char));

    if (!new->value) {
        free(new);
        return false;
    }

    strncpy(new->value, s, str_size);
    *(new->value + str_size) = '\0';
    list_add_tail(&new->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *target = list_entry(head->next, element_t, list);
    list_del(&target->list);

    if (bufsize) {
        strncpy(sp, target->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    return target;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *target = list_entry(head->prev, element_t, list);
    list_del(&target->list);

    if (bufsize) {
        strncpy(sp, target->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    return target;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int size = 0;
    struct list_head *node;
    list_for_each (node, head)
        size++;
    return size;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || !head->next)
        return false;

    struct list_head *rabbit = head->next, *turtle = head->next;

    while (rabbit != head && rabbit->next != head) {
        rabbit = rabbit->next->next;
        turtle = turtle->next;
    }

    list_del(turtle);
    q_release_element(list_entry(turtle, element_t, list));
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head) || list_is_singular(head))
        return false;

    struct list_head *curr = head->next, *next = curr->next;
    bool key = false;

    while (curr != head && next != head) {
        element_t *curr_entry = list_entry(curr, element_t, list);
        element_t *next_entry = list_entry(next, element_t, list);

        while (next != head && !strcmp(curr_entry->value, next_entry->value)) {
            list_del(next);
            q_release_element(next_entry);
            next = curr->next;
            next_entry = list_entry(next, element_t, list);
            key = true;
        }

        if (key) {
            list_del(curr);
            q_release_element(curr_entry);
            key = false;
        }

        curr = next;
        next = next->next;
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;

    struct list_head *first = head->next;

    while (first != head && first->next != head) {
        struct list_head *second = first->next;
        list_del_init(first);
        list_add(first, second);
        first = first->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *prev = head->prev, *curr = head, *next = NULL;

    while (next != head) {
        next = curr->next;
        curr->next = prev;
        curr->prev = next;
        prev = curr;
        curr = next;
    }
}

#if (USE_LINUX_SORT == 0)
/*
 * Merge the two lists in a one sorted list.
 */
static struct list_head *mergelist(struct list_head *list1,
                                   struct list_head *list2)
{
    struct list_head *res = NULL;
    struct list_head **indirect = &res;
    for (struct list_head **node = NULL; list1 && list2;
         *node = (*node)->next) {
        element_t *list1_entry = list_entry(list1, element_t, list);
        element_t *list2_entry = list_entry(list2, element_t, list);
        node = strcmp(list1_entry->value, list2_entry->value) < 0 ? &list1
                                                                  : &list2;
        *indirect = *node;
        indirect = &(*indirect)->next;
    }
    *indirect = (struct list_head *) ((u_int64_t) list1 | (u_int64_t) list2);
    return res;
}

/*
 * Divide the list into several nodes and merge to sorted list.
 * No effect if q is NULL or empty.
 */
static struct list_head *mergesort(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *rabbit = head, *turtle = head;
    while (rabbit && rabbit->next) {
        rabbit = rabbit->next->next;
        turtle = turtle->next;
    }

    struct list_head *mid = turtle;
    turtle->prev->next = NULL;

    struct list_head *left = mergesort(head);
    struct list_head *right = mergesort(mid);
    return mergelist(left, right);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next = mergesort(head->next);

    struct list_head *curr = head, *next = head->next;

    while (next) {
        next->prev = curr;
        curr = next;
        next = next->next;
    }
    curr->next = head;
    head->prev = curr;
}
#endif
