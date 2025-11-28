CC = gcc
CFLAGS = -Wall -Wextra -g -fPIC
LDFLAGS = -shared

# Имена файлов
LIB_NAME = libuthread.so
LIB_SRC = uthread.c
LIB_OBJ = uthread.o

TEST_SRC = test.c
TEST_BIN = test_uthread

.PHONY: all clean test

# По умолчанию собираем библиотеку и тест
all: $(LIB_NAME) $(TEST_BIN)

# Компиляция объектного файла библиотеки
$(LIB_OBJ): $(LIB_SRC) uthread.h
	$(CC) $(CFLAGS) -c $(LIB_SRC) -o $(LIB_OBJ)

# Создание динамической библиотеки
$(LIB_NAME): $(LIB_OBJ)
	$(CC) $(LDFLAGS) -o $(LIB_NAME) $(LIB_OBJ)

# Компиляция тестовой программы
$(TEST_BIN): $(TEST_SRC) $(LIB_NAME) uthread.h
	$(CC) -g $(CFLAGS) $(TEST_SRC) -o $(TEST_BIN) -L. -luthread -Wl,-rpath,.

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f $(LIB_OBJ) $(LIB_NAME) $(TEST_BIN)
