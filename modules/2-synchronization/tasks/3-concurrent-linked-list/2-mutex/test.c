#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdatomic.h>
#include "storage.h"

long long ascending_count = 0;
long long descending_count = 0;
long long equal_count = 0;

long long swap_count_1 = 0;
long long swap_count_2 = 0;
long long swap_count_3 = 0;

int running = 1;

void* thread_ascending(void *arg) {
    Storage *storage = (Storage*)arg;
    
    while (running) {
        int count = 0;
        Node *current = storage->first;
        
        if (current == NULL) continue;
        
        pthread_mutex_lock(&current->sync);
        
        while (current->next != NULL) {
            Node *next = current->next;
            pthread_mutex_lock(&next->sync);
            
            int len1 = strlen(current->value);
            int len2 = strlen(next->value);
            
            if (len1 < len2) {
                count++;
            }
            
            pthread_mutex_unlock(&current->sync);
            current = next;
        }
        
        pthread_mutex_unlock(&current->sync);
        ascending_count++;
    }
    
    return NULL;
}


void* thread_descending(void *arg) {
    Storage *storage = (Storage*)arg;
    
    while (running) {
        int count = 0;
        Node *current = storage->first;
        
        if (current == NULL) continue;
        
        pthread_mutex_lock(&current->sync);
        
        while (current->next != NULL) {
            Node *next = current->next;
            pthread_mutex_lock(&next->sync);
            
            int len1 = strlen(current->value);
            int len2 = strlen(next->value);
            
            if (len1 > len2) {
                count++;
            }
            
            pthread_mutex_unlock(&current->sync);
            current = next;
        }
        
        pthread_mutex_unlock(&current->sync);
        descending_count++;
    }
    
    return NULL;
}


void* thread_equal(void *arg) {
    Storage *storage = (Storage*)arg;
    
    while (running) {
        int count = 0;
        Node *current = storage->first;
        
        if (current == NULL) continue;
        
        pthread_mutex_lock(&current->sync);
        
        while (current->next != NULL) {
            Node *next = current->next;
            pthread_mutex_lock(&next->sync);
            
            int len1 = strlen(current->value);
            int len2 = strlen(next->value);
            
            if (len1 == len2) {
                count++;
            }
            
            pthread_mutex_unlock(&current->sync);
            current = next;
        }
        
        pthread_mutex_unlock(&current->sync);
        equal_count++;
    }
    
    return NULL;
}


typedef struct {
    Storage *storage;
    long long *counter;
} SwapArgs;

void* thread_swap(void *arg) {
    SwapArgs *swap_args = (SwapArgs*)arg;
    Storage *storage = swap_args->storage;
    long long *counter = swap_args->counter;
    
    while (running) {
        if (storage->length < 2) {
            usleep(10);
            continue;
        }
        
        int pos = rand() % (storage->length - 1);
        
        Node *prev = NULL;
        Node *current = storage->first;
        
        for (int i = 0; i < pos && current != NULL; i++) {
            prev = current;
            current = current->next;
        }
        
        if (current == NULL || current->next == NULL) {
            usleep(10);
            continue;
        }
        
        Node *next = current->next;
        
        if (prev != NULL) {
            pthread_mutex_lock(&prev->sync);
            if (prev->next != current) {
                pthread_mutex_unlock(&prev->sync);
                usleep(10);
                continue;
            }
        }
        
        pthread_mutex_lock(&current->sync);
        if (current->next != next) {
            pthread_mutex_unlock(&current->sync);
            if (prev != NULL) pthread_mutex_unlock(&prev->sync);
            usleep(10);
            continue;
        }
        
        pthread_mutex_lock(&next->sync);
        
        int len1 = strlen(current->value);
        int len2 = strlen(next->value);
        
        if (len1 > len2 && rand() % 2 == 0) {
            Node *next_next = next->next;
            
            if (prev != NULL) {
                prev->next = next;
            } else {
                __atomic_store_n(&storage->first, next, __ATOMIC_RELEASE);
            }
            
            next->next = current;
            current->next = next_next;
            
            (*counter)++;
        }
        
        pthread_mutex_unlock(&next->sync);
        pthread_mutex_unlock(&current->sync);
        if (prev != NULL) {
            pthread_mutex_unlock(&prev->sync);
        }
        
        usleep(10);
    }
    
    return NULL;
}



int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <list_size>\n", argv[0]);
        printf("Example sizes: 100, 1000, 10000, 100000\n");
        return 1;
    }
    
    int size = atoi(argv[1]);
    printf("=== Fine-Grained Mutex Implementation ===\n");
    printf("List size: %d\n", size);
    
    srand(time(NULL));
    
    Storage *storage = storage_init(size);
    
    pthread_t t_asc, t_desc, t_eq, t_swap1, t_swap2, t_swap3;
    
    pthread_create(&t_asc, NULL, thread_ascending, storage);
    pthread_create(&t_desc, NULL, thread_descending, storage);
    pthread_create(&t_eq, NULL, thread_equal, storage);
    
    SwapArgs args1 = {storage, &swap_count_1};
    SwapArgs args2 = {storage, &swap_count_2};
    SwapArgs args3 = {storage, &swap_count_3};
    
    pthread_create(&t_swap1, NULL, thread_swap, &args1);
    pthread_create(&t_swap2, NULL, thread_swap, &args2);
    pthread_create(&t_swap3, NULL, thread_swap, &args3);
    
    for (int i = 0; i < 10; i++) {
        sleep(1);
        printf("\n[%d sec]\n", i + 1);
        printf("  Ascending iterations:  %lld\n", ascending_count);
        printf("  Descending iterations: %lld\n", descending_count);
        printf("  Equal iterations:      %lld\n", equal_count);
        printf("  Swap thread 1:         %lld\n", swap_count_1);
        printf("  Swap thread 2:         %lld\n", swap_count_2);
        printf("  Swap thread 3:         %lld\n", swap_count_3);
        printf("  Total swaps:           %lld\n", 
               swap_count_1 + swap_count_2 + swap_count_3);
    }
    
    running = 0;
    
    pthread_join(t_asc, NULL);
    pthread_join(t_desc, NULL);
    pthread_join(t_eq, NULL);
    pthread_join(t_swap1, NULL);
    pthread_join(t_swap2, NULL);
    pthread_join(t_swap3, NULL);
    
    printf("\n=== Final Results ===\n");
    printf("Ascending iterations:  %lld\n", ascending_count);
    printf("Descending iterations: %lld\n", descending_count);
    printf("Equal iterations:      %lld\n", equal_count);
    printf("Total swaps:           %lld\n", 
           swap_count_1 + swap_count_2 + swap_count_3);
    
    storage_destroy(storage);
    
    return 0;
}