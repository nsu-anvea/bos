#define _GNU_SOURCE
#include "storage.h"
#include <time.h>

Storage* storage_init(int size) {
    Storage *storage = malloc(sizeof(Storage));
    storage->first = NULL;
    storage->length = size;
    
    Node *prev = NULL;
    for (int i = 0; i < size; i++) {
        Node *node = malloc(sizeof(Node));
        
        int len = rand() % 99 + 1;
        for (int j = 0; j < len; j++) {
            node->value[j] = 'a' + rand() % 26;
        }
        node->value[len] = '\0';
        
        node->next = NULL;
        pthread_rwlock_init(&node->sync, NULL);
        
        if (prev == NULL) {
            storage->first = node;
        } else {
            prev->next = node;
        }
        prev = node;
    }
    
    return storage;
}

void storage_destroy(Storage *storage) {
    Node *current = storage->first;
    while (current != NULL) {
        Node *next = current->next;
        pthread_rwlock_destroy(&current->sync);
        free(current);
        current = next;
    }
    free(storage);
}