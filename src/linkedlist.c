/* Whiteboard
 * Authors: Mikael Svens and Henrik Alnestig
 * Date: 2014-02-13
 *
 * linkedlist.c.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "linkedlist.h"

/* The following function initializes the linked list by putting zeros
   into the pointers containing the first and last links of the linked
   list. */
void ll_init(linked_list_t * list) {
    list->first = list->last = 0;
    list->count = 0;
}

/* The following function adds a new link to the end of the linked
   list. It allocates memory for it. */
void ll_add(linked_list_t * list, const void * data) {
    link_t * link;

    /* calloc sets the "next" field to zero. */
    link = calloc (1, sizeof (link_t));
    if (! link) {
        fprintf (stderr, "calloc failed.\n");
        exit (EXIT_FAILURE);
    }
    link->data = data;
    if (list->last) {
        /* Join the two final links together. */
        list->last->next = link;
        link->prev = list->last;
        list->last = link;
    }
    else {
        list->first = link;
        list->last = link;
    }
    list->count++;
}

link_t * ll_find(linked_list_t * list, void * data) {
    link_t * link;

    for (link = list->first; link; link = link->next) {
        if (link->data == data) return link;
    }
    return NULL;
}

void ll_delete(linked_list_t * list, link_t * link) {
    link_t * prev;
    link_t * next;

    prev = link->prev;
    next = link->next;
    if (prev) {
        if (next) {
            /* Both the previous and next links are valid, so just
               bypass "link" without altering "list" at all. */
            prev->next = next;
            next->prev = prev;
        }
        else {
            /* Only the previous link is valid, so "prev" is now the
               last link in "list". */
            prev->next = 0;
            list->last = prev;
        }
    }
    else {
        if (next) {
            /* Only the next link is valid, not the previous one, so
               "next" is now the first link in "list". */
            next->prev = 0;
            list->first = next;
        }
        else {
            /* Neither previous nor next links are valid, so the list
               is now empty. */
            list->first = 0;
            list->last = 0;
        }
    }
    free (link);
    list->count--;
}

void ll_traverse(linked_list_t * list, void * data,
                    void (*callback) (void *, void *)) {
    link_t * link;

    for (link = list->first; link; link = link->next) {
        callback (data, (void *) link->data);
    }
}

void ll_traverse_in_reverse(linked_list_t * list, void * data,
                            void (*callback) (void *, void *)) {
    link_t * link;

    for (link = list->last; link; link = link->prev) {
        callback (data, (void *) link->data);
    }
}

void ll_traverse_delete(linked_list_t * list, int (*callback) (void *)) {
    link_t * link;
    link_t * next;

    for (link = list->first; link; link = next) {
	next = link->next;
        if (callback ((void *) link->data)) {
            ll_delete (list, link);
        }
    }
}

/* Free the list's memory. */
void ll_free(linked_list_t * list) {
    link_t * link;
    link_t * next;
    for (link = list->first; link; link = next) {
        /* Store the next value so that we don't access freed
           memory. */
        next = link->next;
        free (link);
    }
}