#ifndef UTHREAD_H
#define UTHREAD_H

#include <ucontext.h>

typedef enum {
    UTHREAD_RUNNING,
    UTHREAD_FINISHED
} uthread_state_t;

typedef struct {
    ucontext_t context;
    void *stack;
    void *retval;
    uthread_state_t state;
} uthread_t;

typedef struct uthread_node {
    uthread_t thread;
    struct uthread_node *next;
} uthread_node_t;

int uthread_create(uthread_t *thread, void *(*start_routine)(void*), void *arg);
void uthread_yield(void);
void uthread_exit(void *retval);

#endif // UTHREAD_H
