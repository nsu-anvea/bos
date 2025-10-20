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
    int counter = 0;
    
    printf("Программа начала выполнение. Начинаю создавать потоки...\n");
    printf("Для остановки нажмите Ctrl+C\n\n");
    
    while (1) {
        pthread_t thread;
        
        if (pthread_create(&thread, NULL, thread_function, NULL)) {
            perror("Ошибка при создании потока");
            break;
        }
        
        counter++;
        printf("Создан поток №%d\n", counter);
		
		pthread_detach(thread);
        
        usleep(50000); // 50ms
    }
    
    return 0;
}
