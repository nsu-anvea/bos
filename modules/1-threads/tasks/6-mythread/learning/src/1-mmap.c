#include <stdio.h>
#include <sys/mman.h>

int main() {
	void *stack = mmap(
		NULL,							// адрес (выбирает ядро)
		1024 * 1024,					// размер (1 МБ)
		PROT_READ | PROT_WRITE,			// права: чтение и запись
		MAP_PRIVATE | MAP_ANONYMOUS,	// приватная анонимная память
		-1,								// файловый дескриптор (-1 для анонимной)
		0								// смещение
	);

	if (stack == MAP_FAILED) {
		perror("mmap failed");
	}
	printf("[INFO]: created stack with addr=%p", stack);
	
	munmap(stack, 1024 * 1024);

	return 0;
}
