#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_function(void *arg) {
    printf("[INFO]: thread have started!\n");
    printf("[INFO]: thread have finished!\n");
    return NULL;
}

int main() {
    pthread_t thread_id;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    int result = pthread_create(&thread_id, &attr, thread_function, NULL);
    if (result != 0) {
        printf("[ERROR]: creating thread\n");
        return 1;
    }

    // sleep(1);
    pthread_exit(NULL);

    return 0;
}