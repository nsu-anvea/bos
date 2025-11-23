# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -O2 -fPIC
LDFLAGS = -shared
LIBS = -lpthread

# Имена файлов
LIB_NAME = libmythread.so
LIB_SRC = mythread.c
LIB_OBJ = mythread.o
HEADER = mythread.h

TEST_SRC = test_mythread.c
TEST_BIN = test_mythread

# Цели
.PHONY: all clean test install

all: $(LIB_NAME) $(TEST_BIN)

# Сборка динамической библиотеки
$(LIB_NAME): $(LIB_OBJ)
	@echo "Linking shared library: $@"
	$(CC) $(LDFLAGS) -o $@ $^

$(LIB_OBJ): $(LIB_SRC) $(HEADER)
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Сборка тестов
$(TEST_BIN): $(TEST_SRC) $(LIB_NAME)
	@echo "Compiling test program: $@"
	$(CC) $(CFLAGS) $< -o $@ -L. -lmythread $(LIBS) -Wl,-rpath,.

# Запуск тестов
test: $(TEST_BIN)
	@echo "Running tests..."
	@./$(TEST_BIN)

# Установка библиотеки в систему (требует sudo)
install: $(LIB_NAME) $(HEADER)
	@echo "Installing library to /usr/local/lib"
	install -m 755 $(LIB_NAME) /usr/local/lib/
	@echo "Installing header to /usr/local/include"
	install -m 644 $(HEADER) /usr/local/include/
	@echo "Running ldconfig..."
	ldconfig
	@echo "Installation complete!"

# Удаление собранных файлов
clean:
	@echo "Cleaning..."
	rm -f $(LIB_NAME) $(LIB_OBJ) $(TEST_BIN)
	@echo "Clean complete!"

# Отладочная сборка
debug: CFLAGS += -g -DDEBUG
debug: clean all

# Справка
help:
	@echo "Available targets:"
	@echo "  all      - Build library and tests (default)"
	@echo "  test     - Build and run tests"
	@echo "  clean    - Remove all built files"
	@echo "  install  - Install library system-wide (needs sudo)"
	@echo "  debug    - Build with debug symbols and DEBUG output"
	@echo "  help     - Show this help"