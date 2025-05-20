#ifndef HTTPS_STATUS_H
#define HTTPS_STATUS_H

// Данный файл содержит константы кодов HTTP-кодов состояния

// Успешный запрос
#define HTTP_OK 200

// Перенаправление на другой URL
#define HTTP_FOUND 302

// Некорректный запрос
#define HTTP_BAD_REQUEST 400

// Метод HTTP не поддерживается для данного маршрута
#define HTTP_METHOD_NOT_ALLOWED 405

// Запрашиваемый ресурс не найден
#define HTTP_NOT_FOUND 404

// Внутренняя ошибка сервера
#define HTTP_INTERNAL_ERROR 500

#endif // HTTPS_STATUS_H
