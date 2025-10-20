#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* thread_function(void* arg) {
    pthread_t thread_id = pthread_self();
    printf("Поток создан. ID: %lu\n", (unsigned long)thread_id);
    
    usleep(100000); // 100ms
    
    printf("Поток завершается. ID: %lu\n", (unsigned long)thread_id);
    return NULL;
}

int main() {
	pthread_attr_t attr;
    int counter = 0;
    
    printf("Программа начала выполнение. Начинаю создавать потоки...\n");

	if (pthread_attr_init(&attr) != 0) {
		perror("pthread_attr_init");
		return EXIT_FAILURE;
	}

	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
		perror("pthread_attr_setdetachstate");
		pthread_attr_destroy(&attr);
		return EXIT_FAILURE;
	}
    
    while (1) {
        pthread_t thread;
        
        if (pthread_create(&thread, &attr, thread_function, NULL)) {
            perror("Ошибка при создании потока");
            break;
        }
        
        counter++;
        printf("Создан поток №%d\n", counter);
		
        usleep(50000); // 50ms
    }
    
    return EXIT_SUCCESS;
}
