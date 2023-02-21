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

    // copy string
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }

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

    // copy string
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }

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
    element_t *element = list_entry(node, element_t, list);

    list_del(node);
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        // add null terminator
        sp[bufsize - 1] = 0;
    }

    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->prev;
    element_t *element = list_entry(node, element_t, list);

    list_del(node);
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        // add null terminator
        sp[bufsize - 1] = 0;
    }

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
    // move point to find the middle node
    for (; afterward != forward && afterward->next != forward;
         afterward = afterward->next, forward = forward->prev)
        ;

    // remove the middle node in queue
    list_del(forward);
    // delete the node
    q_release_element(list_entry(forward, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    bool flag = false;
    element_t *curr_entry, *next_entry;

    list_for_each_entry_safe (curr_entry, next_entry, head, list) {
        while (&next_entry->list != head &&
               !strcmp(curr_entry->value, next_entry->value)) {
            list_del(&next_entry->list);
            q_release_element(next_entry);
            // update next pointer
            next_entry = list_entry(curr_entry->list.next, element_t, list);
            flag = true;
        }

        // need remove current node
        if (flag) {
            list_del(&curr_entry->list);
            q_release_element(curr_entry);
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

    for (struct list_head *second = first->next;
         first != head && second != head;
         first = first->next, second = first->next) {
        // can swap
        list_del_init(first);
        list_add(first, second);
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

/* Connect the previous point to restructure list */
void restructure_list(struct list_head *head)
{
    struct list_head *curr = head, *next = curr->next;
    while (next) {
        next->prev = curr;
        curr = next;
        next = next->next;
    }
    curr->next = head;
    head->prev = curr;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;

    int count = 0;
    struct list_head *sub_head = head->next, *next_head = NULL,
                     *old_tail = head;

    // cut the linked list to be singly-linked list
    head->prev->next = NULL;

    for (struct list_head *sub_tail = head->next; sub_tail;
         sub_tail = sub_tail->next) {
        if (++count == k) {
            next_head = sub_tail->next;
            sub_tail->next = old_tail;
            q_reverse(old_tail);
            // old node connects to the head of new list
            old_tail->next = sub_tail;
            // the new list connect to the next node
            sub_head->next = next_head;
            old_tail = sub_tail = sub_head;
            sub_head = next_head;
            count = 0;
        }
    }
    /* restructure the doubly-linked list */
    restructure_list(head);
}

/* Merge the two lists in a one sorted list. */
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
    *indirect = (struct list_head *) ((size_t) list1 | (size_t) list2);
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

    struct list_head *fast = head, *slow = head;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }

    struct list_head *mid = slow;
    slow->prev->next = NULL;

    struct list_head *left = mergesort(head);
    struct list_head *right = mergesort(mid);
    return mergelist(left, right);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    // cut the linked list to be singly-linked list
    head->prev->next = NULL;
    head->next = mergesort(head->next);
    /* restructure the doubly-linked list */
    restructure_list(head);
}

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
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    for (struct list_head *curr = head->prev, *next = curr->prev; next != head;
         next = curr->prev) {
        element_t *curr_entry = list_entry(curr, element_t, list);
        element_t *next_entry = list_entry(next, element_t, list);

        // if current node is greater than next node
        if (strcmp(curr_entry->value, next_entry->value) > 0) {
            list_del(next);
            q_release_element(next_entry);
        } else
            curr = next;
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    struct list_head *merge = NULL;

    queue_contex_t *head_entry = NULL;
    list_for_each_entry (head_entry, head, chain) {
        // cut the linked list to be singly-linked list
        head_entry->q->prev->next = NULL;
        merge = mergelist(merge, head_entry->q->next);
        head_entry->q->next = head_entry->q;
    }

    head_entry = list_entry(head->next, queue_contex_t, chain);
    head_entry->q->next = merge;

    /* restructure the doubly-linked list */
    restructure_list(head_entry->q);
    return q_size(head_entry->q);
}
