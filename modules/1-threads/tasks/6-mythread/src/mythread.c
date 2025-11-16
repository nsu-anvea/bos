#define _GNU_SOURCE

#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>

#define STACK_SIZE 1024 * 1024



#define INFO_PRINT(fmt, ...) printf("[INFO]: " fmt, ##__VA_ARGS__)

#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) printf("[DEBUG]: " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif



typedef struct {
	size_t 	size;
	void *	arr_ptr;
} mystack_t;

typedef struct mythread_t {
	int 		pid;
	mystack_t *	stack;
	void *		retv;
} mythread_t;

typedef struct {
	void *(*user_fn)(void *);

	void *	user_arg;
	void **	fn_retv;
} thread_wrapper_t;



thread_wrapper_t *create_thread_wrapper(mythread_t *thread, void *(*user_fn)(void *), void *user_arg) {

	thread_wrapper_t *tw = (thread_wrapper_t *)malloc(sizeof(thread_wrapper_t));
	if (!tw) return tw;

	tw->user_fn = user_fn;
	tw->user_arg = user_arg;
	tw->fn_retv = &thread->retv;

	return tw;
}

void delete_thread_wrapper(thread_wrapper_t *tw) {
	free(tw);
}

mystack_t *mystack_create(size_t size) {
	void *stack = mmap(
		NULL,
		size,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1,
		0
	);

	if (stack == MAP_FAILED) {
		perror("mmap failed");
		return NULL;
	}

	mystack_t *s = (mystack_t *)malloc(sizeof(mystack_t));
    if (!s) {
        perror("malloc for mystack_t");
        munmap(stack, size);
        return NULL;
    }
	s->size = size;
	s->arr_ptr = stack;

	return s;
}

int mystack_delete(mystack_t *stack) {
	if (munmap(stack->arr_ptr, stack->size) == -1) {
		perror("munmap failed");
		return -1;
	}
    free(stack);
	return 0;
}

size_t mystack_get_size(mystack_t *mystack) {
	return mystack->size;
}

void *mystack_get_arr_ptr(mystack_t *mystack) {
	return mystack->arr_ptr;
}

int thread_wrapper_fn(void *arg) {

	thread_wrapper_t *tw = (thread_wrapper_t *)arg;
    DEBUG_PRINT("thread_wrapper_fn have got thread_wrapper\n");

    INFO_PRINT("user_fn have started\n");
	void *result = tw->user_fn(tw->user_arg);
    DEBUG_PRINT("user_fn returned: %p\n", result);

    *(tw->fn_retv) = result;
    DEBUG_PRINT("stored result at address: %p, value: %p\n", tw->fn_retv, *(tw->fn_retv));
    INFO_PRINT("user_fn have finished\n");

    delete_thread_wrapper(tw);
    DEBUG_PRINT("thread_wrapper have been deleted\n");

	return 0;
}

int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg) {

	thread->stack = mystack_create(STACK_SIZE);
	if (!thread->stack) {
		perror("mystack_create failed");
		return EXIT_FAILURE;
	}
    DEBUG_PRINT("stack have been created\n");

	thread_wrapper_t *tw = create_thread_wrapper(thread, start_routine, arg);
    if (!tw) {
        perror("create_thread_wrapper failed");
        mystack_delete(thread->stack);
        return EXIT_FAILURE;
    }
    DEBUG_PRINT("thread_wrapper have been created\n");

	thread->pid = clone(
		thread_wrapper_fn,
		thread->stack->arr_ptr + thread->stack->size,
		CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD,
		(void *)tw
	);
	if (thread->pid == -1) {
		perror("clone failed");
		return EXIT_FAILURE;
	}
    DEBUG_PRINT("clone have been executed\n");

	return EXIT_SUCCESS;
}

int mythread_join(mythread_t *thread, void **retv) {
	if (waitpid(thread->pid, NULL, 0) == -1) {
		perror("waitpid failed");
		return EXIT_FAILURE;
	}
    INFO_PRINT("have waited for the mythread to finish\n");

	*retv = thread->retv;
    DEBUG_PRINT("have stored value=%p\n", thread->retv);

	if (mystack_delete(thread->stack) != EXIT_SUCCESS) {
		perror("mystack_delete failed");
	}
    INFO_PRINT("have got the message from the mythread\n");

	return EXIT_SUCCESS;
}

void *mythread_fn(void *arg) {
	printf("\t[MYTHREAD]: I've started!\n");
	printf("\t[MYTHREAD]->[GOT MESSAGE]: %s\n", (char *)arg);
	sleep(1);
	printf("\t[MYTHREAD]: I've finished!\n");
	return (void *)"Hello from mythread!";
}

int main() {
    printf("\n\n\n");

	mythread_t mythread;
	if (mythread_create(&mythread, mythread_fn, (void *)"Hello from main!") != EXIT_SUCCESS) {
		perror("mythread_create failed");
		return EXIT_FAILURE;
	}
    INFO_PRINT("created mythread with pid=%d\n", mythread.pid);

	char *message;
	if (mythread_join(&mythread, (void **)&message) != EXIT_SUCCESS) {
		perror("mythread_join failed");
	}
    INFO_PRINT("mythread with pid=%d have been joined\n", mythread.pid);

	printf("\t[MAIN]->[GOT MESSAGE]: %s\n", message);

    printf("\n\n\n");
	return EXIT_SUCCESS;
}

