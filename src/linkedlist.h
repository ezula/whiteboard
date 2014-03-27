/* Whiteboard
 * Authors: Mikael Svens and Henrik Alnestig
 * Date: 2014-02-13
 *
 * linkedlist.h.
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H


/* A link_t contains one of the links of the linked list. */
typedef struct link {
    const void * data;
    struct link * prev;
    struct link * next;
} link_t;

/* linked_list_t contains a linked list. */
typedef struct linked_list {
    int count;
    link_t * first;
    link_t * last;
} linked_list_t;

void ll_init(linked_list_t * list);
void ll_add(linked_list_t * list, const void * data);
link_t * ll_find(linked_list_t * list, void * data);
void ll_delete(linked_list_t * list, link_t * link);
void ll_traverse(linked_list_t * list, void * data, void (*callback) (void *, void *));
void ll_traverse_in_reverse(linked_list_t * list, void * data, void (*callback) (void *, void *));
void ll_traverse_delete(linked_list_t * list, int (*callback) (void *));
void ll_free(linked_list_t * list);

#endif