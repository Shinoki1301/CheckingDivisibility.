# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -D_GNU_SOURCE \
         -I./mongoose -I./input -I./constants -I./router

# Флаги линковки
LDFLAGS =

# Исходные файлы
SRC = main.c ./mongoose/mongoose.c ./input/input.c ./router/router.c

# Объектные файлы
OBJ = $(SRC:.c=.o)

# Имя исполняемого файла
EXEC = server

.PHONY: all clean run

# Цель по умолчанию
all: $(EXEC)

# Сборка исполняемого файла
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

# Правило для компиляции .c файлов в .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка проекта
clean:
	rm -f $(OBJ) $(EXEC)

# Запуск сервера (предварительно устанавливает переменные окружения)
run: $(EXEC)
	export LOGIN_USER=admin && export LOGIN_PASS=12345 && ./$(EXEC)

