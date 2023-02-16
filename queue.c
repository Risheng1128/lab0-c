#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));
    if (!new)
        return NULL;

    // initialize the head
    INIT_LIST_HEAD(new);
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        list_del(&entry->list);
        q_release_element(entry);
    }

    // free list head
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;

    unsigned int size = strlen(s) + 1;
    element->value = malloc(size * sizeof(char));
    memset(element->value, 0, size);
    if (!element->value) {
        free(element);
        return false;
    }

    // copy string into element
    strncpy(element->value, s, size);
    // initialize the node
    INIT_LIST_HEAD(&element->list);
    // add node into doubly-linked list at head
    list_add(&element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;

    unsigned int len = strlen(s);
    element->value = malloc((len + 1) * sizeof(char));
    if (!element->value) {
        free(element);
        return false;
    }

    // copy string into element
    strncpy(element->value, s, len);
    element->value[len] = 0;
    // initialize the node
    INIT_LIST_HEAD(&element->list);
    // add node into doubly-linked list at tail
    list_add_tail(&element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->next;
    list_del(node);

    element_t *element = list_entry(node, element_t, list);
    strncpy(sp, element->value, bufsize - 1);
    sp[bufsize - 1] = 0;

    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->prev;
    list_del(node);

    element_t *element = list_entry(node, element_t, list);
    strncpy(sp, element->value, bufsize - 1);
    sp[bufsize - 1] = 0;

    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int count = 0;
    struct list_head *node;
    list_for_each (node, head)
        count++;

    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head)
        return false;

    struct list_head *afterward = head->next, *forward = head->prev;
    while (afterward != forward && afterward->next != forward) {
        afterward = afterward->next;
        forward = forward->prev;
    }

    // remove the middle node in queue
    list_del(forward);
    // delete the node
    element_t *element = list_entry(forward, element_t, list);
    q_release_element(element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    bool flag = false;
    struct list_head *curr = head->next;

    while (curr != head) {
        struct list_head *next = curr->next;
        element_t *curr_entry = list_entry(curr, element_t, list);

        while (next != head) {
            element_t *next_entry = list_entry(next, element_t, list);

            // strings are same
            if (!strcmp(curr_entry->value, next_entry->value)) {
                list_del(next);
                q_release_element(next_entry);
                flag = true;
            }
            next = curr->next;
        }

        curr = curr->next;

        // need to delete current node
        if (flag) {
            element_t *del = curr_entry;
            list_del(&del->list);
            q_release_element(del);
            flag = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;

    struct list_head *first = head->next;

    while (first != head && first->next != head) {
        // can swap
        struct list_head *second = first->next;
        list_del_init(first);
        list_add(first, second);
        first = first->next;
    }
}

/* Reverse elements in queue */
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

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
