#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#ifndef strlcpy
#define strlcpy(dst, src, sz) snprintf((dst), (sz), "%s", (src))
#endif

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */

typedef struct {
    int count;
    struct list_head list;
} q_head;


struct list_head *q_new()
{
    q_head *temp = malloc(sizeof(q_head));
    if (!temp) {
        return NULL;
    }
    temp->count = 0;
    INIT_LIST_HEAD(&temp->list);
    return &temp->list;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l) {
        return;
    }

    struct list_head *temp = l->next;
    while (l != temp) {
        l->next = temp->next;
        free(list_entry(temp, element_t, list)->value);
        free(list_entry(temp, element_t, list));
        temp = l->next;
    }

    free(list_entry(l, q_head, list));
    return;
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
    if (!head) {
        return false;
    }
    element_t *temp_member = malloc(sizeof(element_t));
    if (!temp_member) {
        return false;
    }
    int count = 0;
    for (; s[count]; count++) {
    };
    temp_member->value = malloc(count + 1 * sizeof(char));
    if (!temp_member->value) {
        free(temp_member);
        return false;
    }
    strlcpy(temp_member->value, s, count + 1);
    list_add(&temp_member->list, head);
    list_entry(head, q_head, list)->count += 1;
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
    if (!head) {
        return false;
    }
    element_t *temp_member = malloc(sizeof(element_t));
    if (!temp_member) {
        return false;
    }
    int count = 0;
    for (; s[count]; count++) {
    };
    temp_member->value = malloc(count + 1 * sizeof(char));
    if (!temp_member->value) {
        free(temp_member);
        return false;
    }
    strlcpy(temp_member->value, s, count + 1);
    list_add_tail(&temp_member->list, head);
    list_entry(head, q_head, list)->count += 1;
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
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *temp = list_entry(head->next, element_t, list);
    list_del(head->next);
    if (sp) {
        strlcpy(sp, temp->value, bufsize);
    }
    list_entry(head, q_head, list)->count -= 1;
    return temp;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *temp = list_entry(head->prev, element_t, list);
    list_del(head->prev);
    if (sp) {
        strlcpy(sp, temp->value, bufsize);
    }
    list_entry(head, q_head, list)->count -= 1;
    return temp;
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
    if (!head) {
        return 0;
    }
    return list_entry(head, q_head, list)->count;
}


void q_delete_element(struct list_head *node)
{
    if (!node) {
        return;
    }
    list_del(node);
    free(list_entry(node, element_t, list)->value);
    free(list_entry(node, element_t, list));
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
    if (!head || list_empty(head)) {
        return false;
    }
    int n = list_entry(head, q_head, list)->count-- / 2;
    // list_entry(head, q_head, list)->count -= 1;
    struct list_head *ptr = head->next;
    while (n--) {
        ptr = ptr->next;
    }
    q_delete_element(ptr);
    // list_del(ptr);
    // free(list_entry(ptr, element_t, list)->value);
    // free(list_entry(ptr, element_t, list));

    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,Goes back
 * to a NULL queue list is guaranteed to be sorted in ascending order.
 */

// https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
bool q_delete_dup(struct list_head *head)
{
    if (!head) {
        return false;
    }

    if (list_empty(head) || list_is_singular(head)) {
        return true;
    }
    int count = list_entry(head, q_head, list)->count;
    struct list_head *ptr = head->next;
    bool kill_self = false;
    while (ptr != head && ptr->next != head) {
        while (ptr->next != head &&
               !strcmp(list_entry(ptr, element_t, list)->value,
                       list_entry(ptr->next, element_t, list)->value)) {
            q_delete_element(ptr->next);
            count--;
            kill_self = true;
        }
        ptr = ptr->next;
        if (kill_self) {
            q_delete_element(ptr->prev);
            kill_self = false;
            count--;
        }
    }
    list_entry(head, q_head, list)->count = count;
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void list_swap(struct list_head **list_addr1, struct list_head **list_addr2)
{
    struct list_head *temp = *list_addr2;
    *list_addr2 = *list_addr1;
    *list_addr1 = temp;
}
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *ptr = head;
    while (ptr->next != head && ptr->next->next != head) {
        struct list_head *first = ptr->next;
        struct list_head *second = ptr->next->next;
        first->next = second->next;
        ptr->next = second;
        ptr->next->next = first;
        ptr = ptr->next->next;
    }
    ptr = head;
    while (ptr->next != head) {
        ptr->next->prev = ptr;
        ptr = ptr->next;
    }
    head->prev = ptr;
    return;
    // https://leetcode.com/problems/swap-nodes-in-pairs/
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
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *temp = head;
    do {
        // struct list_head *t = temp->next;
        // temp->next = temp->prev;
        // temp->prev = t;
        list_swap(&temp->next, &temp->prev);
        temp = temp->prev;
    } while (temp != head);
}
/*
 * cite from
 * https://hackmd.io/@sysprog/c-linked-list#%E6%A1%88%E4%BE%8B%E6%8E%A2%E8%A8%8E-LeetCode-21-Merge-Two-Sorted-Lists
 */
struct list_head *merge_two_list(struct list_head *q_1, struct list_head *q_2)
{
    if (!q_1 || !q_2)
        return (struct list_head *) ((__UINTPTR_TYPE__) q_1 |
                                     (__UINTPTR_TYPE__) q_2);
    struct list_head *head = NULL, **ptr = &head, **node;
    for (node = NULL; q_1 && q_2; *node = (*node)->next) {
        node = (strcmp(list_entry(q_1, element_t, list)->value,
                       list_entry(q_2, element_t, list)->value) <= 0)
                   ? &q_1
                   : &q_2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr =
        (struct list_head *) ((__UINTPTR_TYPE__) q_1 | (__UINTPTR_TYPE__) q_2);
    return head;
}
struct list_head *list_mergesort(struct list_head *head)
{
    if (!head || !head->next) {
        return head;
    }
    struct list_head *slow = head;
    struct list_head *fast = head->next;
    for (; fast && fast->next; fast = fast->next->next) {
        slow = slow->next;
    }
    fast = slow->next;
    slow->next = NULL;
    struct list_head *left = list_mergesort(head);
    struct list_head *right = list_mergesort(fast);
    return merge_two_list(left, right);
}
/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */

void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    head->prev->next = NULL;
    head->next = list_mergesort(head->next);
    struct list_head *ptr = head;
    while (ptr->next) {
        ptr->next->prev = ptr;
        ptr = ptr->next;
    }
    ptr->next = head;
    head->prev = ptr;
    return;
}
