// Copyright 2021 Spirache Eduard

#ifndef CIRCULARLIST_H_
#define CIRCULARLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dll_node_t dll_node_t;
struct dll_node_t
{
    void* data;
    dll_node_t *prev, *next;
};

typedef struct load_balancer load_balancer;
struct load_balancer {
	unsigned int size;
    unsigned int data_size;
    dll_node_t* head;
};

/*
 * Acestea sunt functiile pe care trebuie sa le implementam.
 * Implementarea acestora se va face in LinkedList.c .
 */

load_balancer *
loader_create(unsigned int data_size)
{
    load_balancer *list = malloc(sizeof(load_balancer));
    list->head = NULL;
    list->data_size = data_size;
    list->size = 0;
    return list;
}

dll_node_t *
loader_get_nth_node(load_balancer *list, unsigned int n)
{
    unsigned int i;

    dll_node_t *current = list->head;

    for (i = 0; i < n; i++) {
        current = current->next;
    }

    return current;
}

void
loader_add_nth_node(load_balancer *list, unsigned int n, void *data)
{
    dll_node_t *node = malloc(sizeof(dll_node_t));
    dll_node_t *current = list->head;

    node->data = data;

    if (list->size == 0) {
        list->head = node;
        list->head->next = list->head;
        list->head->prev = list->head;
    } else if (n == 0 && list->size > 0) {
        node->prev = current->prev;
        node->next = current;
        current->prev = node;
        node->prev->next = node;
        list->head = node;
    } else if (n >= list->size) {
        node->prev = list->head->prev;
        node->next = list->head;
        list->head->prev = node;
        node->prev->next = node;
    } else {
        current = loader_get_nth_node(list, n);
        node->prev = current->prev;
        node->next = current;
        node->next->prev = node;
        node->prev->next = node;
    }

    list->size++;
}

dll_node_t *
loader_remove_nth_node(load_balancer *list, unsigned int n)
{
    dll_node_t *current = list->head;
    dll_node_t *before = NULL;
    dll_node_t *after = NULL;

    if (list->size == 1) {
        list->head->next = list->head;
        list->head->prev = list->head;
    } else if (list->size == 2) {
        if (n == 1) {
            current = current->next;
            list->head->next = list->head;
            list->head->prev = list->head;
        } else {
            after = current->next;
            list->head = after;
            after->next = after;
            after->prev = after;
        }
    } else if (list->size > 2 && n == 0) {
        current->prev->next = current->next;
        current->next->prev = current->prev;
        list->head = current->next;
    } else {
        current = loader_get_nth_node(list, n);
        before = current->prev;
        after = current->next;
        before->next = after;
        after->prev = before;
    }

    list->size--;
    return current;
}

#endif  // CIRCULARLIST_H_
