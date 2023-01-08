// Copyright 2021 Spirache Eduard
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#include "Hashtable.h"

hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	unsigned int i;
	hashtable_t *table = malloc(sizeof(hashtable_t));
	DIE(table == NULL, "Eroare alocare!");

	table->size = 0;
	table->hmax = hmax;
	table->compare_function = compare_function;
	table->hash_function = hash_function;

	table->buckets = malloc(hmax * sizeof(linked_list_t*));
	DIE(table->buckets == NULL, "Eroare alocare!");

	for(i = 0; i < hmax; i ++) {
		table->buckets[i] = ll_create(sizeof(struct info));
	}
	return table;
}

/*
 * Functie care compara hash-ul elementelor din tabelul serverului
 * urmator serverului nou adaugat cu hash-ul serverului nou,
 * care mai apoi copiaza elementele respective pe noul server si le
 * sterge din celalalt.
*/
void
ht_cut(hashtable_t* first, hashtable_t* second, unsigned int second_hash,
	unsigned int first_hash, unsigned int count, unsigned int size) {
	unsigned int i;
	int ok;
	ll_node_t *node, *next_node;
	struct info* info;

	for(i = 0; i < first->hmax; i ++) {
		node = first->buckets[i]->head;
		while(node != NULL) {
			next_node = node->next;
			info = (struct info*)node->data;
			// comparam key-ul hashuit din tabel cu hashul serverului
			ok = 0;
			if (count > 0 && count < size - 1) {
				if (second_hash > first->hash_function(info->key)) {
					ok = 1;
				}
			} else if (count >= size - 1) {
				if (first->hash_function(info->key) < first_hash &&
					first->hash_function(info->key) < second_hash) {
					ok = 0;
				} else if (first->hash_function(info->key) > first_hash &&
							first->hash_function(info->key) > second_hash) {
					ok = 0;
				} else  if (second_hash > first->hash_function(info->key)){
					ok = 1;
				}
			} else if (count == 0) {
				if (second_hash > first->hash_function(info->key) ||
					first_hash < first->hash_function(info->key)) {
					ok = 1;
				}
			}
			if(ok) {
				ht_put(second, info->key, strlen((char*)info->key) + 1,
						info->value, strlen((char*)info->value) + 1);
				ht_remove_entry(first, info->key);
			}
			node = next_node;
		}
	}
}

/*
 * Functie care muta toate datele de pe un server pe celalalt
*/
void
ht_move(hashtable_t* first, hashtable_t* second) {
	unsigned int i;
	ll_node_t *node;
	struct info* info;

	for(i = 0; i < first->hmax; i ++) {
		node = first->buckets[i]->head;
		while(node != NULL) {
			info = (struct info*)node->data;
			ht_put(second, info->key, strlen((char*)info->key) + 1,
					info->value, strlen((char*)info->value) + 1);
			node = node->next;
		}
	}
}

void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	linked_list_t* bucket;
	ll_node_t* current;
	struct info *tmp, *new;
	unsigned int index;
	if(ht == NULL) {
		fprintf(stderr, "Eroare");
		return;
	}

	index = ht->hash_function(key) % ht->hmax;

	bucket = ht->buckets[index];

	current = bucket->head;

	while(current != NULL) {
		tmp = (struct info*)current->data;
		if(ht->compare_function(key, tmp->key) == 0) {
			free(tmp->value);
			tmp->value = malloc(value_size);
			memcpy(tmp->value, value, value_size);
			ht->size++;
            return;
		}
		current = current->next;
	}
	new = malloc(sizeof(struct info));

	new->value = malloc(value_size);
	DIE(new->value == NULL, "Eroare alocare");
	memcpy(new->value, value, value_size);

	new->key = malloc(key_size);
	DIE(new->key == NULL, "Eroare alocare");
	memcpy(new->key, key, key_size);

	ll_add_nth_node(bucket, 0, new);
	ht->size++;
}

void *
ht_get(hashtable_t *ht, void *key)
{
	unsigned int index;
	ll_node_t* current;
	linked_list_t* bucket;
	if(ht == NULL) {
		fprintf(stderr, "Eroare");
	}

	index = ht->hash_function(key) % ht->hmax;

	bucket = ht->buckets[index];
	current = bucket->head;

	while(current != NULL) {
		struct info* info = (struct info*)current->data;
		if(ht->compare_function(key, info->key) == 0) {
			return info->value;
		}
		current = current->next;
	}
	return NULL;
}

int
ht_has_key(hashtable_t *ht, void *key)
{
	linked_list_t* bucket;
	ll_node_t* current;
	unsigned int index;
	if(ht == NULL) {
		fprintf(stderr, "Eroare");
		return 0;
	}

	index = ht->hash_function(key) % ht->hmax;

	bucket = ht->buckets[index];
	current = bucket->head;

	while(current != NULL) {
		struct info* info = (struct info*)current->data;
		if(ht->compare_function(key, info->key) == 0) {
			return 1;
		}
		current = current->next;
	}
	return 0;
}

void
ht_remove_entry(hashtable_t *ht, void *key)
{
	int count;
	linked_list_t* bucket;
	ll_node_t* current;
	struct info* info;
	unsigned int index;
	if(ht == NULL) {
		fprintf(stderr, "Eroare");
		return;
	}
	count = 0;
	index = ht->hash_function(key) % ht->hmax;

	bucket = ht->buckets[index];
	current = bucket->head;

	while(current != NULL) {
		count++;
		info = (struct info*)current->data;
		if(ht->compare_function(key, info->key) == 0) {
			free(info->value);
			free(info->key);
			current = ll_remove_nth_node(bucket, count);
			free(current->data);
			free(current);
			ht->size--;
			return;
		}
		current = current->next;
	}
}

void
ht_free(hashtable_t *ht)
{
	unsigned int i;
	ll_node_t* current = NULL;
	if (ht == NULL) {
		fprintf(stderr, "Eroare");
		return;
	}
	for (i = 0; i < ht->hmax; i ++) {
		current = ht->buckets[i]->head;
		while(current != NULL) {
        	free(((struct info*)current->data)->key);
        	free(((struct info*)current->data)->value);
			current = current->next;
		}
		ll_free(&ht->buckets[i]);
	}
	free(ht->buckets);
	free(ht);
}
