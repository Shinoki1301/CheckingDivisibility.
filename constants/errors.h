#ifndef ERRORS_H
#define ERRORS_H

typedef enum {
	ERROR_OK = 0,             // Нет ошибки
	ERR_FILE_NOT_FOUND = 1,   // Файл не найден
	ERR_DIVISION_BY_ZERO = 2  // Деление на ноль
} ErrorCode;

#endif // ERRORS_H
