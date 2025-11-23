#include "mythread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>  /* Для pthread_mutex - синхронизация между потоками */

/* Цвета для вывода */
#define COLOR_GREEN  "\033[0;32m"
#define COLOR_RED    "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_RESET  "\033[0m"

#define TEST_PASS(name) printf(COLOR_GREEN "✓ PASSED: %s" COLOR_RESET "\n", name)
#define TEST_FAIL(name) printf(COLOR_RED "✗ FAILED: %s" COLOR_RESET "\n", name)
#define TEST_INFO(fmt, ...) printf(COLOR_YELLOW "[INFO] " COLOR_RESET fmt "\n", ##__VA_ARGS__)

/* Глобальный счётчик для теста синхронизации */
static int global_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

/* --- Тест 1: Базовое создание и join --- */
void *simple_thread_fn(void *arg) {
    char *msg = (char *)arg;
    printf("  [Thread] Received: %s\n", msg);
    return (void *)"Response from thread";
}

int test_simple_create_join(void) {
    TEST_INFO("Test 1: Simple create + join");
    
    mythread_t thread;
    int ret = mythread_create(&thread, simple_thread_fn, (void *)"Hello from main");
    if (ret != 0) {
        perror("  mythread_create");
        TEST_FAIL("Simple create+join");
        return -1;
    }
    
    char *response;
    ret = mythread_join(&thread, (void **)&response);
    if (ret != 0) {
        perror("  mythread_join");
        TEST_FAIL("Simple create+join");
        return -1;
    }
    
    printf("  [Main] Received: %s\n", response);
    TEST_PASS("Simple create+join");
    return 0;
}

/* --- Тест 2: Множественные потоки --- */
#define NUM_THREADS 10

void *counter_thread_fn(void *arg) {
    int thread_id = (int)(long)arg;  // Передаём ID по значению, не по указателю
    
    for (int i = 0; i < 100; i++) {
        pthread_mutex_lock(&counter_mutex);
        global_counter++;
        pthread_mutex_unlock(&counter_mutex);
    }
    
    printf("  [Thread %d] Finished incrementing\n", thread_id);
    
    /* Возвращаем ID потока */
    int *result = malloc(sizeof(int));
    *result = thread_id;
    return (void *)result;
}

int test_multiple_threads(void) {
    TEST_INFO("Test 2: Multiple threads (%d threads)", NUM_THREADS);
    
    mythread_t threads[NUM_THREADS];
    
    global_counter = 0;
    
    /* Создаём потоки - передаём ID напрямую как значение */
    for (int i = 0; i < NUM_THREADS; i++) {
        int ret = mythread_create(&threads[i], counter_thread_fn, (void *)(long)i);
        if (ret != 0) {
            perror("  mythread_create");
            TEST_FAIL("Multiple threads - create");
            /* Ждём уже созданные потоки */
            for (int j = 0; j < i; j++) {
                mythread_join(&threads[j], NULL);
            }
            return -1;
        }
    }
    
    /* Ждём все потоки */
    int success = 1;
    for (int i = 0; i < NUM_THREADS; i++) {
        int *result;
        int ret = mythread_join(&threads[i], (void **)&result);
        if (ret != 0) {
            perror("  mythread_join");
            success = 0;
        } else {
            printf("  [Main] Thread %d joined, returned ID: %d\n", i, *result);
            free(result);
        }
    }
    
    printf("  [Main] Final counter value: %d (expected: %d)\n", 
           global_counter, NUM_THREADS * 100);
    
    if (!success || global_counter != NUM_THREADS * 100) {
        TEST_FAIL("Multiple threads");
        return -1;
    }
    
    TEST_PASS("Multiple threads");
    return 0;
}

/* --- Тест 3: Поток с длительной работой --- */
void *long_running_thread_fn(void *arg) {
    int seconds = *(int *)arg;
    printf("  [Thread] Sleeping for %d seconds...\n", seconds);
    sleep(seconds);
    printf("  [Thread] Woke up!\n");
    return NULL;
}

int test_long_running_thread(void) {
    TEST_INFO("Test 3: Long-running thread");
    
    mythread_t thread;
    int sleep_time = 2;
    
    int ret = mythread_create(&thread, long_running_thread_fn, &sleep_time);
    if (ret != 0) {
        perror("  mythread_create");
        TEST_FAIL("Long-running thread");
        return -1;
    }
    
    printf("  [Main] Waiting for thread...\n");
    ret = mythread_join(&thread, NULL);
    if (ret != 0) {
        perror("  mythread_join");
        TEST_FAIL("Long-running thread");
        return -1;
    }
    
    TEST_PASS("Long-running thread");
    return 0;
}

/* --- Тест 4: Проверка обработки ошибок --- */
int test_error_handling(void) {
    TEST_INFO("Test 4: Error handling");
    
    mythread_t thread;
    
    /* Тест 1: NULL для start_routine */
    int ret = mythread_create(&thread, NULL, NULL);
    if (ret == 0) {
        TEST_FAIL("Error handling - NULL function should fail");
        mythread_join(&thread, NULL);
        return -1;
    }
    printf("  [Main] NULL function correctly rejected (errno=%d)\n", errno);
    
    /* Тест 2: Некорректный join */
    mythread_t invalid_thread = {.pid = -1, .stack = NULL, .retv = NULL};
    ret = mythread_join(&invalid_thread, NULL);
    if (ret == 0) {
        TEST_FAIL("Error handling - invalid thread should fail");
        return -1;
    }
    printf("  [Main] Invalid thread join correctly rejected (errno=%d)\n", errno);
    
    TEST_PASS("Error handling");
    return 0;
}

/* --- Тест 5: Последовательное создание/завершение --- */
void *sequential_thread_fn(void *arg) {
    int id = *(int *)arg;
    printf("  [Thread %d] Running\n", id);
    usleep(100000);  /* 100ms */
    return NULL;
}

int test_sequential_creation(void) {
    TEST_INFO("Test 5: Sequential thread creation/joining");
    
    for (int i = 0; i < 5; i++) {
        mythread_t thread;
        int thread_id = i;
        
        int ret = mythread_create(&thread, sequential_thread_fn, &thread_id);
        if (ret != 0) {
            perror("  mythread_create");
            TEST_FAIL("Sequential creation");
            return -1;
        }
        
        ret = mythread_join(&thread, NULL);
        if (ret != 0) {
            perror("  mythread_join");
            TEST_FAIL("Sequential creation");
            return -1;
        }
        
        printf("  [Main] Thread %d completed\n", i);
    }
    
    TEST_PASS("Sequential creation");
    return 0;
}

/* --- Тест 6: Стресс-тест с большим количеством потоков --- */
#define STRESS_THREADS 50

void *stress_thread_fn(void *arg) {
    int id = *(int *)arg;
    /* Минимальная работа - просто запускаемся и завершаемся */
    (void)id;  /* Подавляем предупреждение о неиспользуемой переменной */
    return NULL;
}

int test_stress(void) {
    TEST_INFO("Test 6: Stress test (%d threads)", STRESS_THREADS);
    
    mythread_t threads[STRESS_THREADS];
    int thread_ids[STRESS_THREADS];
    
    /* Создаём много потоков */
    int created = 0;
    for (int i = 0; i < STRESS_THREADS; i++) {
        thread_ids[i] = i;
        int ret = mythread_create(&threads[i], stress_thread_fn, &thread_ids[i]);
        if (ret != 0) {
            perror("  mythread_create");
            printf("  [Main] Failed to create thread %d\n", i);
            break;
        }
        created++;
    }
    
    /* Ждём все созданные потоки */
    int joined = 0;
    for (int i = 0; i < created; i++) {
        int ret = mythread_join(&threads[i], NULL);
        if (ret != 0) {
            perror("  mythread_join");
        } else {
            joined++;
        }
    }
    
    printf("  [Main] Created: %d, Joined: %d\n", created, joined);
    
    if (created == STRESS_THREADS && joined == STRESS_THREADS) {
        TEST_PASS("Stress test");
        return 0;
    } else {
        TEST_FAIL("Stress test");
        return -1;
    }
}

/* --- Главная функция --- */
int main(void) {
    printf("\n" COLOR_YELLOW "=== MYTHREAD LIBRARY TEST SUITE ===" COLOR_RESET "\n\n");
    
    int tests_passed = 0;
    int tests_total = 0;
    
    #define RUN_TEST(test) \
        do { \
            tests_total++; \
            if (test() == 0) tests_passed++; \
            printf("\n"); \
        } while(0)
    
    RUN_TEST(test_simple_create_join);
    RUN_TEST(test_multiple_threads);
    RUN_TEST(test_long_running_thread);
    RUN_TEST(test_error_handling);
    RUN_TEST(test_sequential_creation);
    RUN_TEST(test_stress);
    
    printf(COLOR_YELLOW "=== TEST SUMMARY ===" COLOR_RESET "\n");
    printf("Passed: %d/%d\n", tests_passed, tests_total);
    
    if (tests_passed == tests_total) {
        printf(COLOR_GREEN "All tests passed! ✓" COLOR_RESET "\n\n");
        return EXIT_SUCCESS;
    } else {
        printf(COLOR_RED "Some tests failed! ✗" COLOR_RESET "\n\n");
        return EXIT_FAILURE;
    }
}