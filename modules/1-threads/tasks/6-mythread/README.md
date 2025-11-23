# MyThread Library

Реализация пользовательских ядерных потоков на основе системного вызова `clone()` - аналог `pthread_create()`.

## Структура проекта

```
.
├── include/
│   └── mythread.h          # Заголовочный файл библиотеки
├── src/
│   └── mythread.c          # Реализация библиотеки
├── test/
│   └── test_mythread.c     # Комплексные тесты
├── learning/
│   └── src/                # Учебные примеры
│       ├── 1-mmap.c
│       └── 2-clone.c
├── Makefile
└── README.md
```

## Сборка и запуск

```bash
# Собрать библиотеку и тесты
make

# Запустить тесты
make test

# Собрать учебные примеры
make learning

# Отладочная сборка (с DEBUG логами)
make debug

# Очистка
make clean
```

## API

### mythread_create

```c
int mythread_create(mythread_t *thread, 
                    void *(*start_routine)(void *), 
                    void *arg);
```

Создаёт новый поток.

**Параметры:**
- `thread` - структура потока (заполняется функцией)
- `start_routine` - функция для выполнения
- `arg` - аргумент для функции

**Возвращает:** `0` при успехе, `-1` при ошибке (устанавливает `errno`)

### mythread_join

```c
int mythread_join(mythread_t *thread, void **retv);
```

Ожидает завершения потока.

**Параметры:**
- `thread` - структура потока
- `retv` - указатель для возвращаемого значения (может быть `NULL`)

**Возвращает:** `0` при успехе, `-1` при ошибке

## Пример использования

```c
#include <stdio.h>
#include "mythread.h"

void *thread_fn(void *arg) {
    printf("Thread: %s\n", (char *)arg);
    return (void *)"Done!";
}

int main() {
    mythread_t thread;
    
    if (mythread_create(&thread, thread_fn, "Hello") != 0) {
        perror("mythread_create");
        return 1;
    }
    
    char *result;
    if (mythread_join(&thread, (void **)&result) != 0) {
        perror("mythread_join");
        return 1;
    }
    
    printf("Result: %s\n", result);
    return 0;
}
```

**Компиляция:**
```bash
gcc -Iinclude your_program.c -o your_program -Lbuild -lmythread -Wl,-rpath,build
```

## Детали реализации

### Технологии

- **clone()** с флагами: `CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND`
- **mmap()** для выделения стека (1 МБ на поток)
- **waitpid()** для ожидания завершения

### Что это: процесс или поток?

**Ядерный поток (kernel thread)** - задача с общим адресным пространством:
- ✅ Общая память между потоками
- ✅ Общие файловые дескрипторы
- ✅ Общие глобальные переменные
- ⚠️ Разные PID (как у процессов)

Это то же, что делает `pthread` внутри, но без дополнительной функциональности (TLS, cleanup handlers и т.д.).

### Ограничения

- Нет `mythread_cancel()`
- Нет атрибутов потоков
- Нет thread-local storage (TLS)
- Нет detached режима

## Тесты

Библиотека включает 6 комплексных тестов:

1. **Simple create+join** - базовая функциональность
2. **Multiple threads** - 10 параллельных потоков с синхронизацией
3. **Long-running thread** - проверка корректности ожидания
4. **Error handling** - обработка некорректных параметров
5. **Sequential creation** - циклическое создание/завершение
6. **Stress test** - 50 потоков одновременно

## mythread_cancel - как бы реализовать?

### Вариант 1: Сигналы (асинхронная отмена)

```c
int mythread_cancel(mythread_t *thread) {
    return kill(thread->pid, SIGUSR1);  // Отправляем сигнал
}

// В thread_wrapper_fn устанавливаем обработчик:
signal(SIGUSR1, cancel_handler);
```

**Минусы:** нет точек отмены, нет cleanup handlers

### Вариант 2: Флаг отмены (кооперативная)

```c
typedef struct mythread_t {
    // ...
    volatile sig_atomic_t *cancel_flag;
} mythread_t;

int mythread_cancel(mythread_t *thread) {
    *(thread->cancel_flag) = 1;
    return 0;
}

// Поток проверяет флаг периодически:
void mythread_testcancel() {
    if (*cancel_flag) _exit(0);
}
```

**Минусы:** требует явной проверки от программиста

### Вариант 3: Комбинированный (лучший)

Сочетание сигналов и флага:
- Флаг для deferred cancellation
- Сигнал для asynchronous cancellation
- Функции `mythread_testcancel()` для точек отмены

## Логирование

**INFO логи** (всегда включены):
- Старт/завершение пользовательской функции
- Ожидание потока

**DEBUG логи** (только с `make debug`):
- Адреса памяти
- PID созданных потоков
- Детали каждого этапа

## Лицензия

Учебный проект для изучения системного программирования в Linux.