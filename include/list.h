#ifndef _LIST_H
#define _LIST_H

#include <stddef.h>
#include <stdbool.h>
#include <panic.h>

// Structure for list head and elements
struct list_head {
    struct list_head *prev;
    struct list_head *next;
};

typedef struct list_head list_t;
typedef struct list_head list_elem_t;

// Macros for container and iteration
#define LIST_CONTAINER(head, container, field)                                 \
    ((container *) ((char *)(head) - offsetof(container, field)))

#define LIST_FOR_EACH(elem, list, container, field)                            \
    for (container *elem = LIST_CONTAINER((list)->next, container, field),     \
                   *__next = NULL;                                             \
         ((list_t*)&elem->field != (list)                                               \
          && (__next = LIST_CONTAINER(elem->field.next, container, field)));   \
         elem = __next)

// Initialize a list
static inline void list_init(list_t *list) {
    PANIC_IF(list == NULL); // Ensure list is not NULL
    list->prev = list;
    list->next = list;
}

// Check if the list is empty
static inline bool list_is_empty(list_t *list) {
    PANIC_IF(list == NULL); // Ensure list is not NULL
    return list->next == list;
}

// Check if an element is in the list
static inline bool list_contains(list_t *list, list_elem_t *elem) {
    PANIC_IF(list == NULL); // Ensure list is not NULL
    PANIC_IF(elem == NULL); // Ensure the element is not NULL

    list_elem_t *node = list->next;
    while (node != list) {
        if (node == elem) {
            return true;
        }
        node = node->next;
    }
    return false;
}

// Append an element to the back of the list
static inline void list_push_back(list_t *list, list_elem_t *new_elem) {
    PANIC_IF(list == NULL);   // Ensure list is not NULL
    PANIC_IF(new_elem == NULL); // Ensure the new element is not NULL
    PANIC_IF(list_contains(list, new_elem)); // Ensure the element is not already in the list

    new_elem->prev = list->prev;
    new_elem->next = list;
    list->prev->next = new_elem;
    list->prev = new_elem;
}

// Pop the first element from the list
static inline list_elem_t *list_pop_front(list_t *list) {
    PANIC_IF(list == NULL); // Ensure list is not NULL
    if (list_is_empty(list)) {
        return NULL; // List is empty
    }

    list_elem_t *head = list->next;
    list->next = head->next;
    head->next->prev = list;

    head->prev = NULL; // Invalidate the element
    head->next = NULL; // Invalidate the element
    return head;
}

// Remove an element from the list
static inline void list_remove(list_elem_t *elem) {
    PANIC_IF(elem == NULL); // Ensure the element is not NULL
    if (elem->next == NULL && elem->prev == NULL) {
        return; // The element is not in any list
    }

    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;

    // Invalidate the element
    elem->prev = NULL;
    elem->next = NULL;
}

// Check the length of the list
static inline size_t list_len(list_t *list) {
    PANIC_IF(list == NULL); // Ensure list is not NULL
    size_t len = 0;
    list_elem_t *node = list->next;
    while (node != list) {
        len++;
        node = node->next;
    }
    return len;
}

// Macro for popping the front element and getting its container
#define LIST_POP_FRONT(list, container, field)                                 \
    ({                                                                         \
        list_elem_t *__elem = list_pop_front(list);                           \
        (__elem) ? LIST_CONTAINER(__elem, container, field) : NULL;           \
    })

#endif // __LIST_H__
