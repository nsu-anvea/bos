#include "uthread.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STACK_SIZE (16 * 1024)


static uthread_node_t *thread_list = NULL;
static uthread_node_t *current_thread = NULL;
static ucontext_t main_context;
static int scheduler_started = 0;


static void thread_wrapper(void *(*start_routine)(void*), void *arg) {
    void *retval = start_routine(arg);
    uthread_exit(retval);
}


int uthread_create(uthread_t *thread, void *(*start_routine)(void*), void *arg) {
    if (!thread || !start_routine) {
        return -1;
    }
    
    thread->stack = malloc(STACK_SIZE);
    if (!thread->stack) {
        perror("malloc");
        return -1;
    }
    
    thread->state = UTHREAD_RUNNING;
    thread->retval = NULL;
    
    if (getcontext(&thread->context) == -1) {
        perror("getcontext");
        free(thread->stack);
        return -1;
    }
    
    thread->context.uc_stack.ss_sp = thread->stack;
    thread->context.uc_stack.ss_size = STACK_SIZE;
    thread->context.uc_link = NULL;
    
    makecontext(&thread->context, (void(*)())thread_wrapper, 2, start_routine, arg);
    
    uthread_node_t *node = malloc(sizeof(uthread_node_t));
    if (!node) {
        perror("malloc");
        free(thread->stack);
        return -1;
    }
    
    node->thread = *thread;
    
    if (thread_list == NULL) {
        thread_list = node;
        node->next = node;
    } else {
        uthread_node_t *last = thread_list;
        while (last->next != thread_list) {
            last = last->next;
        }
        node->next = thread_list;
        last->next = node;
    }
    
    return 0;
}


void uthread_yield(void) {
    if (!thread_list) {
        return;
    }
    
    if (!scheduler_started) {
        scheduler_started = 1;
        current_thread = thread_list;
    
        swapcontext(&main_context, &current_thread->thread.context);
        return;
    }
    
    uthread_node_t *prev = current_thread;
    uthread_node_t *next = current_thread->next;
    
    while (next->thread.state == UTHREAD_FINISHED && next != current_thread) {
        next = next->next;
    }
    
    if (next->thread.state == UTHREAD_FINISHED && next == current_thread) {
        setcontext(&main_context);
        return;
    }
    
    if (current_thread->thread.state == UTHREAD_FINISHED) {
        current_thread = next;
        setcontext(&current_thread->thread.context);
        return;
    }
    
    current_thread = next;
    swapcontext(&prev->thread.context, &current_thread->thread.context);
}


void uthread_exit(void *retval) {
    if (!current_thread) {
        return;
    }

    current_thread->thread.state = UTHREAD_FINISHED;
    current_thread->thread.retval = retval;
    
    uthread_yield();
}