#define _GNU_SOURCE

#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>

int thread_function(void *arg) {
	printf("\t[THREAD]: I've started!\n");
	printf("\t[THREAD][PRINT]: %s\n", (char *)arg);
	sleep(1);
	printf("\t[THREAD]: I've finished!\n");
	return 0;
}

int main() {

	int size = 1024 * 1024;
	void *stack = mmap(
		NULL,							// адрес (выбирает ядро)
		size,
		PROT_READ | PROT_WRITE,			// права: чтение и запись
		MAP_PRIVATE | MAP_ANONYMOUS,	// приватная анонимная память
		-1,								// файловый дескриптор (-1 для анонимной)
		0								// смещение
	);

	if (stack == MAP_FAILED) {
		perror("mmap failed");
	}

	printf("[INFO]: created stack with addr=%p\n", stack);
	// Если убрать \n этот и следующий print могут продублироваться,
	// предположительно из-за того, что clone может копировать буфер,
	// и перед clone мы не сбросили буфер с помощью \n.

	
	int pid = clone(
		thread_function,
		stack + size,
		CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD,
		(void *)"Hello, World!"
	);
	
	if (pid == -1) {
		perror("clone failed");
		return 1;
	}
	
	printf("[INFO]: Created thread with pid=%d\n", pid);


	if (waitpid(pid, NULL, 0) == -1) {
		perror("waitpid failed");
	}


	printf("[INFO]: Thread finished\n");
	
	if (munmap(stack, size) == -1) {
		perror("munmap failed");
	}

	return 0;
}

