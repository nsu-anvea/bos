#define _GNU_SOURCE

#include "mythread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)  /* 1 МБ на стек */

/* Система логирования */
#define INFO_PRINT(fmt, ...) printf("[INFO]: " fmt, ##__VA_ARGS__)

#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) printf("[DEBUG]: " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif

/* Внутренняя структура стека */
struct mystack_t {
    size_t  size;
    void *  arr_ptr;
};

/* Обёртка для передачи данных в новый поток */
typedef struct {
    void *(*user_fn)(void *);
    void *  user_arg;
    void ** fn_retv;
} thread_wrapper_t;

/* --- Функции работы со стеком --- */

static mystack_t *mystack_create(size_t size) {
    void *stack = mmap(
        NULL,
        size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK,  /* MAP_STACK - подсказка ядру */
        -1,
        0
    );

    if (stack == MAP_FAILED) {
        DEBUG_PRINT("mmap failed for stack\n");
        return NULL;
    }
    DEBUG_PRINT("mmap succeeded, stack allocated at %p\n", stack);

    mystack_t *s = (mystack_t *)malloc(sizeof(mystack_t));
    if (!s) {
        perror("malloc for mystack_t");
        munmap(stack, size);  /* Откатываем изменения при ошибке */
        return NULL;
    }

    s->size = size;
    s->arr_ptr = stack;
    DEBUG_PRINT("stack structure created, size=%zu\n", size);

    return s;
}

static int mystack_delete(mystack_t *stack) {
    if (!stack) {
        return 0;  /* NULL - не ошибка */
    }

    DEBUG_PRINT("deleting stack at %p\n", stack->arr_ptr);
    int ret = 0;
    if (munmap(stack->arr_ptr, stack->size) == -1) {
        perror("munmap failed");
        ret = -1;
    }
    free(stack);
    DEBUG_PRINT("stack deleted\n");
    return ret;
}

/* --- Обёртка потока --- */

static thread_wrapper_t *create_thread_wrapper(mythread_t *thread, 
                                               void *(*user_fn)(void *), 
                                               void *user_arg) {
    thread_wrapper_t *tw = (thread_wrapper_t *)malloc(sizeof(thread_wrapper_t));
    if (!tw) {
        DEBUG_PRINT("malloc failed for thread_wrapper\n");
        return NULL;
    }

    tw->user_fn = user_fn;
    tw->user_arg = user_arg;
    tw->fn_retv = &thread->retv;

    DEBUG_PRINT("thread_wrapper created at %p\n", tw);
    return tw;
}

/* Функция-обёртка, выполняющаяся в новом потоке */
static int thread_wrapper_fn(void *arg) {
    thread_wrapper_t *tw = (thread_wrapper_t *)arg;
    DEBUG_PRINT("thread_wrapper_fn have got thread_wrapper\n");
    
    /* Выполняем пользовательскую функцию */
    INFO_PRINT("user_fn have started\n");
    void *result = tw->user_fn(tw->user_arg);
    DEBUG_PRINT("user_fn returned: %p\n", result);
    
    /* Сохраняем результат */
    *(tw->fn_retv) = result;
    DEBUG_PRINT("stored result at address: %p, value: %p\n", tw->fn_retv, *(tw->fn_retv));
    INFO_PRINT("user_fn have finished\n");
    
    /* Освобождаем память обёртки */
    free(tw);
    DEBUG_PRINT("thread_wrapper have been deleted\n");
    
    return 0;
}

/* --- Публичные функции --- */

int mythread_create(mythread_t *thread, 
                    void *(*start_routine)(void *), 
                    void *arg) {
    if (!thread || !start_routine) {
        errno = EINVAL;
        return -1;
    }

    /* Инициализируем поля */
    thread->pid = -1;
    thread->stack = NULL;
    thread->retv = NULL;

    /* Создаём стек */
    thread->stack = mystack_create(STACK_SIZE);
    if (!thread->stack) {
        perror("mystack_create failed");
        return -1;  /* errno уже установлен mmap/malloc */
    }
    DEBUG_PRINT("stack have been created\n");

    /* Создаём обёртку */
    thread_wrapper_t *tw = create_thread_wrapper(thread, start_routine, arg);
    if (!tw) {
        perror("create_thread_wrapper failed");
        mystack_delete(thread->stack);
        thread->stack = NULL;
        return -1;  /* errno установлен malloc */
    }
    DEBUG_PRINT("thread_wrapper have been created\n");

    /* Клонируем процесс */
    thread->pid = clone(
        thread_wrapper_fn,
        (char *)thread->stack->arr_ptr + thread->stack->size,  /* Стек растёт вниз */
        CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD,
        (void *)tw
    );

    if (thread->pid == -1) {
        int saved_errno = errno;  /* Сохраняем errno */
        perror("clone failed");
        free(tw);
        mystack_delete(thread->stack);
        thread->stack = NULL;
        errno = saved_errno;
        return -1;
    }
    DEBUG_PRINT("clone have been executed, pid=%d\n", thread->pid);

    return 0;
}

int mythread_join(mythread_t *thread, void **retv) {
    if (!thread || thread->pid == -1) {
        errno = EINVAL;
        return -1;
    }

    /* Ожидаем завершения потока */
    if (waitpid(thread->pid, NULL, 0) == -1) {
        perror("waitpid failed");
        return -1;  /* errno установлен waitpid */
    }
    INFO_PRINT("have waited for the mythread to finish\n");

    /* Возвращаем результат, если нужно */
    if (retv) {
        *retv = thread->retv;
        DEBUG_PRINT("have stored value=%p\n", thread->retv);
    }

    /* Освобождаем стек */
    if (mystack_delete(thread->stack) == -1) {
        perror("mystack_delete failed");
        thread->stack = NULL;
        return -1;
    }
    thread->stack = NULL;
    INFO_PRINT("have got the message from the mythread\n");

    return 0;
}