// Copyright 2021 Spirache Eduard
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "utils.h"

#define MAX_SIZE 64

int
compare_function(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

unsigned int hash_function(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

server_memory* init_server_memory() {
	server_memory *server = malloc(sizeof(server_memory));
	DIE(server == NULL, "Eroare alocare!");

	server->id = 0;
	server->replica_id = 0;
	server->tag = 0;
	server->table = ht_create(MAX_SIZE, hash_function, compare_function);
	return server;
}

void server_store(server_memory* server, char* key, char* value) {
	ht_put(server->table, key, strlen(key) + 1,
		value, strlen(value) + 1);
}

void server_remove(server_memory* server, char* key) {
	ht_remove_entry(server->table, key);
}

char* server_retrieve(server_memory* server, char* key) {
	return ht_get(server->table, key);
}

void free_server_memory(server_memory* server) {
	ht_free(server->table);
	free(server);
}
