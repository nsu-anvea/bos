#include "uthread.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STACK_SIZE (16 * 1024)

// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ПЛАНИРОВЩИКА
// static = видны только внутри этого файла (инкапсуляция)
static uthread_node_t *thread_list = NULL;     // Голова кольцевого списка
static uthread_node_t *current_thread = NULL;  // Текущий выполняющийся поток

static void thread_wrapper(void *(*start_routine)(void*), void *arg) {
    void *retval = start_routine(arg);
    uthread_exit(retval);
}

int uthread_create(uthread_t *thread, void *(*start_routine)(void*), void *arg) {
    if (!thread || !start_routine) {
        return -1;
    }
    
    thread->stack = (void *)malloc(sizeof(void) * STACK_SIZE);
    if (!thread->stack) {
        perror("malloc");
        return -1;
    }
    
    thread->state = UTHREAD_RUNNING;  // Поток готов к выполнению
    thread->retval = NULL;            // Пока нет возвращаемого значения

    // Сначала получаем текущий контекст как шаблон
    if (getcontext(&thread->context) == -1) {
        perror("getcontext");
        free(thread->stack);
        return -1;
    }
    
    thread->context.uc_stack.ss_sp = thread->stack;
    thread->context.uc_stack.ss_size = STACK_SIZE;
    thread->context.uc_link = NULL;  // После завершения вернёмся в NULL (выход)
    
    // Модифицируем контекст: теперь он будет выполнять thread_wrapper
    // thread_wrapper вызовет start_routine(arg) и потом uthread_exit
    makecontext(&thread->context, (void(*)())thread_wrapper, 2, start_routine, arg);
    
    uthread_node_t *node = malloc(sizeof(uthread_node_t));
    if (!node) {
        perror("malloc");
        free(thread->stack);
        return -1;
    }
    
    node->thread = *thread;
    printf("%p\n", &node->thread);

    if (thread_list == NULL) {
        thread_list = node;
        node->next = node;
        current_thread = node;
    } else {
        node->next = current_thread->next; //           (3)->(2)->(1)->(3)
        current_thread->next = node;
    }


    
    return 0;
}

void uthread_yield(void) {
    if (!current_thread || !thread_list) {
        return;
    }
    
    uthread_node_t *prev = current_thread;
    uthread_node_t *next = current_thread->next;
    printf("prev=%p\nnext=%p\n\n", prev, next);
    while (next->thread.state == UTHREAD_FINISHED && next != current_thread) {
        next = next->next;  // Пропускаем завершённые потоки
    }
    
    // Если все потоки завершились, кроме текущего
    if (next->thread.state == UTHREAD_FINISHED && next == current_thread) {
        return;  // Больше нечего переключать
    }
    
    current_thread = next;
    
    // КЛЮЧЕВАЯ ФУНКЦИЯ: сохраняем контекст prev и загружаем контекст next
    // Это атомарная операция: save + restore
    swapcontext(&prev->thread.context, &current_thread->thread.context);
    
    // ВАЖНО: после swapcontext мы вернёмся сюда, когда кто-то переключится обратно!
}

void uthread_exit(void *retval) {
    if (!current_thread) {
        return;
    }

    current_thread->thread.state = UTHREAD_FINISHED;
    current_thread->thread.retval = retval;
    
    // Освобождаем стек (опционально, можно сделать в cleanup-функции)
    // free(current_thread->thread.stack);
    
    uthread_yield();
}
