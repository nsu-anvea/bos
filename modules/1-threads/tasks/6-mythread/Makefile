# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -shared -fPIC
LIBS = -lpthread

# Директории
SRC_DIR = src
TEST_DIR = test
INCLUDE_DIR = include
BUILD_DIR = build
LEARNING_DIR = learning/src

# Файлы
LIB_NAME = $(BUILD_DIR)/libmythread.so
LIB_SRC = $(SRC_DIR)/mythread.c
LIB_OBJ = $(BUILD_DIR)/mythread.o

TEST_SRC = $(TEST_DIR)/test_mythread.c
TEST_BIN = $(BUILD_DIR)/test_mythread

LEARNING_SRCS = $(wildcard $(LEARNING_DIR)/*.c)
LEARNING_BINS = $(patsubst $(LEARNING_DIR)/%.c,$(BUILD_DIR)/%,$(LEARNING_SRCS))

# Цели
.PHONY: all clean test learning

all: $(LIB_NAME) $(TEST_BIN)

# Создание директории build
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Сборка библиотеки
$(LIB_NAME): $(LIB_OBJ) | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(LIB_OBJ): $(LIB_SRC) $(INCLUDE_DIR)/mythread.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# Сборка тестов
$(TEST_BIN): $(TEST_SRC) $(LIB_NAME) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@ -L$(BUILD_DIR) -lmythread $(LIBS) -Wl,-rpath,$(BUILD_DIR)

# Запуск тестов
test: $(TEST_BIN)
	@echo "Running tests..."
	@./$(TEST_BIN)

# Сборка learning примеров
learning: $(LEARNING_BINS)

$(BUILD_DIR)/%: $(LEARNING_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Очистка
clean:
	rm -rf $(BUILD_DIR)

# Отладочная сборка
debug: CFLAGS += -g -DDEBUG
debug: clean all