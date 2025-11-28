#include "uthread.h"
#include <stdio.h>

void *thread_func1(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 5; i++) {
        printf("  [Поток %d] Итерация %d\n", id, i);
        uthread_yield();
    }
    printf("  [Поток %d] Завершился\n", id);
    return NULL;
}

void *thread_func2(void *arg) {
    int id = *(int *)arg;
    
    for (int i = 0; i < 3; i++) {
        printf("  [Поток %d] Шаг %d\n", id, i);
        uthread_yield();
    }    
    printf("  [Поток %d] Завершился\n", id);
    return NULL;
}

void *thread_func3(void *arg) {
    int id = *(int *)arg;
    
    for (int i = 0; i < 4; i++) {
        printf("  [Поток %d] Работа %d\n", id, i);
        uthread_yield();
    }    
    printf("  [Поток %d] Завершился\n", id);
    return NULL;
}

int main() {
    printf("\n\n===[ Демонстрация пользовательских потоков ]===\n\n");
    
    uthread_t t1, t2, t3;
    int id1 = 1, id2 = 2, id3 = 3;
    
    printf("Создание потоков...\n");
    if (uthread_create(&t1, thread_func1, &id1) != 0) {
        fprintf(stderr, "Ошибка создания потока 1\n");
        return 1;
    }
    
    if (uthread_create(&t2, thread_func2, &id2) != 0) {
        fprintf(stderr, "Ошибка создания потока 2\n");
        return 1;
    }
    
    if (uthread_create(&t3, thread_func3, &id3) != 0) {
        fprintf(stderr, "Ошибка создания потока 3\n");
        return 1;
    }
    
    printf("\nЗапуск планировщика...\n\n");
    uthread_yield();
    
    printf("\n\n=== Все потоки завершились ===\n\n\n");
    
    return 0;
}