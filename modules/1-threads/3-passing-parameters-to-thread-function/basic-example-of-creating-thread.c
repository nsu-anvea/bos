#include <stdio.h>
#include <pthread.h>

void *thread_function(void *arg) {
    printf("[INFO]: thread have started!\n");
    printf("[INFO]: thread have finished!\n");
    return NULL;
}

int main() {
    pthread_t thread_id;

    int result = pthread_create(&thread_id, NULL, thread_function, NULL);
    if (result != 0) {
        printf("[ERROR]: creating thread\n");
        return 1;
    }

    pthread_join(thread_id, NULL);

    return 0;
}