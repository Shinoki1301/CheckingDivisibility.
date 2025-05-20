#include "router.h"
#include <string.h>

// Структура, описывающая один маршрут: путь и связанный с ним идентификатор маршрута
typedef struct {
    const char* path;  // Путь (URI)
    Route       route; // Какой маршрут ему соответствует
} RouteEntry;

// Таблица маршрутов
static const RouteEntry route_table[] = {
    { "/style.css", ROUTE_STYLES },
    { "/logout",     ROUTE_LOGOUT },
    { "/login",      ROUTE_LOGIN  },
    { "/div",        ROUTE_DIV_FORM },
    { "/div-form",   ROUTE_DIV_FORM },
    { "/div-submit", ROUTE_DIV_SUBMIT },
    { "/",           ROUTE_ROOT   },
};

// Определяет маршрут по URI HTTP-запроса
Route resolve_route(struct mg_http_message* hm) {
    Route resolved = ROUTE_UNKNOWN;

    size_t count = sizeof(route_table) / sizeof(route_table[0]); 
    for (size_t i = 0; i < count; ++i) {
        // Сравнение URI запроса с каждым путем в таблице
        if (mg_strcmp(hm->uri, mg_str(route_table[i].path)) == 0) {
            resolved = route_table[i].route;
	    i = count;
        }
    }
    // Если путь не найден — возвращаем ROUTE_UNKNOWN
    return resolved;
}

