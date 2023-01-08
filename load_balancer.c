// Copyright 2021 Spirache Eduard
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"
#include "CircularList.h"

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;
    return hash;
}

load_balancer* init_load_balancer() {
    load_balancer *main = loader_create(sizeof(server_memory));
    return main;
}

void loader_store(load_balancer* main, char* key, char* value, int* server_id) {
    unsigned int i;
    server_memory* current_server;
    dll_node_t* current;

    if (main == NULL || main->size == 0) {
        return;
    }

    current = main->head;

    // daca cheia e mai mare decat ultimul server stocam pe primul
    if (hash_function_key(key) > ((server_memory*)(current->prev->data))->tag) {
        current_server = (server_memory*)(current->data);
    } else {
        for (i = 0; i < main->size; i ++) {
            if (hash_function_key(key) > ((server_memory*)current->data)->tag) {
                current = current->next;
            } else {
                break;
            }
        }
        current_server = (server_memory*)(current->data);
    }
    *server_id = current_server->id;
    server_store(current_server, key, value);
}


char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
	unsigned int i;
    dll_node_t* current;
    char* data;

    current = main->head;
    for (i = 0; i < main->size; i ++) {
        data = server_retrieve(((server_memory*)current->data), key);
        if (data != NULL) {
            *server_id = ((server_memory*)current->data)->id;
            break;
        }
        current = current->next;
    }
	return data;
}

void loader_add_server(load_balancer* main, int server_id) {
    int i;
    unsigned int hashed, index, count;
    dll_node_t* current;
    server_memory* added_server;
    for (i = 0; i < 3; i ++) {
        added_server = init_server_memory();
        // copiez id-ul si tag-ul
        added_server->id = server_id;
        added_server->replica_id = i;
        hashed = server_id + i * 100000;
        added_server->tag = hash_function_servers(&hashed);

        // iterez pana la serverul inainte de care adaug
        // serverul nou (serverul cu hash mai mare decat cel nou adaugat)
        count = 0;
        if (main->size > 0) {
            current = main->head;
            for (index = 0; index < main->size; index ++) {
                if (added_server->tag >= ((server_memory*)current->data)->tag) {
                    current = current->next;
                    count++;
                } else {
                    break;
                }
            }
            main->size++;
            ht_cut(((server_memory*)current->data)->table, added_server->table,
                    added_server->tag, ((server_memory*)current->data)->tag,
                    count, main->size);
            main->size--;
        }
        loader_add_nth_node(main, count, (server_memory*)added_server);
    }
}

void loader_remove_server(load_balancer* main, int server_id) {
    int i, replica_id;
    int count;
    unsigned int index;
    dll_node_t* current;

    if (main == NULL || main->size == 0) {
        return;
    }

	for (i = 0; i < 3; i ++) {
        replica_id = i;
        current = main->head;

        if (main->size <= 3) {
            current = loader_remove_nth_node(main, 0);
            free_server_memory(current->data);
            free(current);
            break;
        }
        count = 0;
        for (index = 0; index < main->size; index ++) {
            if (((server_memory*)current->data)->id == server_id &&
                ((server_memory*)current->data)->replica_id == replica_id) {
                    current = loader_remove_nth_node(main, count);
                    // mut obiectele din tabel
                    ht_move(((server_memory*)current->data)->table,
                        ((server_memory*)current->next->data)->table);
                    // eliberez memoria pentru tabel
                    free_server_memory(current->data);
                    free(current);
                    break;
                }
            count++;
            current = current->next;
        }
    }
}

void free_load_balancer(load_balancer* main) {
    unsigned int i;

    if (main == NULL) {
        return;
    }

    dll_node_t *after = NULL;
    dll_node_t *current = main->head;

    for (i = 0; i < main->size; i++) {
        after = current->next;
        free_server_memory(current->data);
        free(current);
        current = after;
    }
    free(main);
}
