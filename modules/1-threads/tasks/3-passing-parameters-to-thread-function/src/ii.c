#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct some_struct {
    int some_int;
    char *some_char_ptr;
};

struct some_struct *create_some_struct() {
    struct some_struct *st = (struct some_struct *) malloc(sizeof(struct some_struct));
    st->some_char_ptr = (char *) malloc(sizeof(char));
    return st;
}

void init_struct(struct some_struct *st, int i, char c) {
	st->some_int = i;
	*st->some_char_ptr = c;
}

void fini_struct(struct some_struct *st) {
	free(st->some_char_ptr);
}

void destroy_struct(struct some_struct *st) {
    fini_struct(st);
    free(st);
}

void cleanup_handler(void *arg) {
    struct some_struct *st = (struct some_struct *) arg;
    printf("[CLEANUP]: freeing memory\n");
    destroy_struct(st);
}

void *thread_function(void *arg) {
    printf("[INFO]: thread have started!\n");
	struct some_struct *st = (struct some_struct *) arg;

    pthread_cleanup_push(cleanup_handler, st);

	printf("\t[THREAD]: some_int=%d\n", st->some_int);
	pthread_exit(NULL);
	printf("\t[THREAD]: some_char=%c\n", *st->some_char_ptr);

    pthread_cleanup_pop(1);

    printf("[INFO]: thread have finished!\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t thread_id;

	struct some_struct *st = create_some_struct();
	init_struct(st, 1, '@');

    int result = pthread_create(&thread_id, NULL, thread_function, (void *) st);
    if (result != 0) {
        printf("[ERROR]: creating thread\n");
        return 1;
    }

    pthread_detach(thread_id);

    pthread_exit(NULL);

    return 0;
}
