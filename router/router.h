#ifndef ROUTER_H
#define ROUTER_H

#include "mongoose.h"  // Подключаем библиотеку Mongoose для работы с HTTP

// Перечисление всех возможных маршрутов (идентификаторы маршрутов)
typedef enum {
    ROUTE_UNKNOWN,     // Неизвестный маршрут
    ROUTE_STYLES,      // /styles.css — запрос CSS-стилей
    ROUTE_LOGOUT,      // /logout — выход
    ROUTE_LOGIN,       // /login — вход (POST)
    ROUTE_DIV_FORM,    // /div или /div-form — форма деления
    ROUTE_DIV_SUBMIT,  // /div-submit — проверка делимости
    ROUTE_ROOT         // / — корневая страница
} Route;

// Функция, определяющая маршрут по URI из запроса
Route resolve_route(struct mg_http_message* hm);

#endif // ROUTER_H

