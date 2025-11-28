CC = gcc
CFLAGS = -Wall -Wextra -fPIC
CFLAGS_DEBUG = -Wall -Wextra -g -O0 -fPIC
LDFLAGS = -shared

# Структура директорий
BUILD_DIR = build
LIB_DIR = $(BUILD_DIR)/lib
BIN_DIR = $(BUILD_DIR)/bin
OBJ_DIR = $(BUILD_DIR)/obj

# Имена файлов
LIB_NAME = libuthread.so
LIB_SRC = uthread.c
LIB_OBJ = $(OBJ_DIR)/uthread.o

TEST_SRC = test.c
TEST_BIN = test_uthread

# Пути к финальным файлам
LIB_PATH = $(LIB_DIR)/$(LIB_NAME)
BIN_PATH = $(BIN_DIR)/$(TEST_BIN)

.PHONY: all clean test debug dirs

# По умолчанию собираем release версию
all: dirs $(LIB_PATH) $(BIN_PATH)

# Создание структуры директорий
dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)

# Компиляция объектного файла (release)
$(LIB_OBJ): $(LIB_SRC) uthread.h
	$(CC) $(CFLAGS) -c $(LIB_SRC) -o $(LIB_OBJ)

# Создание динамической библиотеки
$(LIB_PATH): $(LIB_OBJ)
	$(CC) $(LDFLAGS) -o $(LIB_PATH) $(LIB_OBJ)

# Компиляция тестовой программы (release)
$(BIN_PATH): $(TEST_SRC) $(LIB_PATH) uthread.h
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(BIN_PATH) -L$(LIB_DIR) -luthread -Wl,-rpath,$(LIB_DIR)

# Debug сборка с символами отладки
debug: CFLAGS = $(CFLAGS_DEBUG)
debug: clean dirs
	@echo "=== Сборка в режиме DEBUG ==="
	$(CC) $(CFLAGS) -c $(LIB_SRC) -o $(LIB_OBJ)
	$(CC) $(LDFLAGS) -o $(LIB_PATH) $(LIB_OBJ)
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(BIN_PATH) -L$(LIB_DIR) -luthread -Wl,-rpath,$(LIB_DIR)
	@echo "=== Debug сборка готова в $(BUILD_DIR)/ ==="
	@echo "Для отладки: gdb $(BIN_PATH)"

# Запуск теста
test: all
	@echo "=== Запуск тестов ==="
	LD_LIBRARY_PATH=$(LIB_DIR) $(BIN_PATH)

# Очистка
clean:
	rm -rf $(BUILD_DIR)

# Информация о структуре проекта
info:
	@echo "Структура проекта:"
	@echo "  $(BUILD_DIR)/"
	@echo "    ├── lib/          - динамические библиотеки"
	@echo "    ├── bin/          - исполняемые файлы"
	@echo "    └── obj/          - объектные файлы"
	@echo ""
	@echo "Команды:"
	@echo "  make          - сборка release версии"
	@echo "  make debug    - сборка с отладочной информацией"
	@echo "  make test     - сборка и запуск тестов"
	@echo "  make clean    - очистка build директории"