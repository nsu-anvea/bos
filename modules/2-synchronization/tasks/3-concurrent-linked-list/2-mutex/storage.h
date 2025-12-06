#ifndef __STORAGE_H__
#define __STORAGE_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct _Node {
    char value[100];
    struct _Node *next;
    pthread_mutex_t sync;
} Node;

typedef struct _Storage {
    Node *first;
    int length;
} Storage;

Storage *storage_init(int size);
void storage_destroy(Storage *storage);

#endif // __STORAGE_H__