#include "./constants/constants.h"
#include "./constants/error_messages.h"
#include "./constants/errors.h"
#include "./constants/http_status.h"
#include "./input/input.h"
#include "./mongoose/mongoose.h"
#include "./router/router.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool authenticated = false;

// Обработка выхода
static void handle_logout(struct mg_connection* c) {

    authenticated = false;

    char* response = read_file(PATH_LOGIN_HTML);
    mg_http_reply(c, HTTP_OK, 
		  CONTENT_TYPE_HTML, 
		  "%s", response);
    free(response);
}

// Обработка логина
static void handle_login(struct mg_connection*   c, 
			 struct mg_http_message* hm) {

    char        username[100], password[100];
    const char* expected_user = getenv("LOGIN_USER");
    const char* expected_pass = getenv("LOGIN_PASS");

    char* response = NULL;
    bool  success  = false;

    if (expected_user && expected_pass) {
        mg_http_get_var(&hm->body, "username", username, sizeof (username));
        mg_http_get_var(&hm->body, "password", password, sizeof (password));

        success = (strcmp (username, expected_user) == 0 &&
                   strcmp (password, expected_pass) == 0);
    }

    if (success) {
        authenticated = true;
        mg_http_reply(c, HTTP_FOUND, "Location: /div\r\n", "");
    } else {
        response = read_file(PATH_ERROR_HTML);
        mg_http_reply(c, HTTP_OK, 
		      CONTENT_TYPE_HTML, 
		      "%s", response);
        free(response);
    }
}

// Обработка GET-запроса (форма проверки делимости)
static void handle_div_page(struct mg_connection* c) {

    char* response = NULL;

    if (authenticated) {
        response = read_file(PATH_DIV_HTML);
        if (response) {
            mg_http_reply(c, HTTP_OK, 
			  CONTENT_TYPE_HTML, 
			  "%s", response);
            free (response);
        } else {
            mg_http_reply(c, HTTP_INTERNAL_ERROR, "",
			  ERROR_INTERNAL_SERVER);
        }
    } else {
        mg_http_reply(c, HTTP_FOUND, 
		      "Location: /\r\n", "");
    }
}

// Обработка GET-запроса (проверка делимости)
static void handle_get_div_check(struct mg_connection*   c,
                                 struct mg_http_message* hm) {

    char dividend_str[50], 
    	 divisor_str[50];
    mg_http_get_var(&hm->query, "dividend", dividend_str, sizeof (dividend_str));
    mg_http_get_var(&hm->query, "divisor", divisor_str, sizeof (divisor_str));

    long dividend = strtol(dividend_str, NULL, 10);
    long divisor  = strtol(divisor_str, NULL, 10);

    if (divisor == 0) {
        mg_http_reply(c, HTTP_BAD_REQUEST, 
		      CONTENT_TYPE_TEXT_PLAIN,
                      ERROR_DIVISION_BY_ZERO);
        return;
    }

    long remainder = dividend % divisor;
    char result[200];

    if (remainder == 0) {
        snprintf(result, sizeof (result), "%ld делится на %ld без остатка",
                 dividend, divisor);
    } else {
        snprintf(result, sizeof (result), "%ld не делится на %ld. Остаток: %ld",
                 dividend, divisor, remainder);
    }

    mg_http_reply(c, HTTP_OK, 
		  CONTENT_TYPE_TEXT_PLAIN, 
		  "%s", result);
}

// Обработка запроса CSS
static void handle_css_request(struct mg_connection* c) {

    char* response = read_file(PATH_CSS_STYLES);
    if (response) {
        mg_http_reply(c, HTTP_OK, 
		      CONTENT_TYPE_CSS, 
		      "%s", response);
        free(response);
    } else {
        mg_http_reply(c, HTTP_INTERNAL_ERROR, "",
		      ERROR_INTERNAL_SERVER);
    }
}

// Обработчик событий
static void fn(struct mg_connection* c, 
	       int ev,
	       void* ev_data) {

    bool                    processed = false;
    struct mg_http_message* hm        = NULL;
    Route                   route     = ROUTE_UNKNOWN;
    char*                   response  = NULL;

    if (ev == MG_EV_HTTP_MSG) {
        hm    = (struct mg_http_message*) ev_data;
        route = resolve_route(hm);
        // Обработка неавторизованных пользователей
        if (!authenticated && 
	    route != ROUTE_LOGIN && 
	    route != ROUTE_STYLES) {

            response = read_file(PATH_LOGIN_HTML);
            mg_http_reply(c, HTTP_OK, 
			  CONTENT_TYPE_HTML, 
			  "%s", response);
            processed = true;
        }
        // Обработка маршрутов, если запрос еще не обработан
        if (!processed) {
            switch (route) {
            case ROUTE_STYLES:
                handle_css_request(c);
                break;

            case ROUTE_LOGOUT:
                handle_logout(c);
                break;

            case ROUTE_LOGIN:
                if (mg_strcmp (hm->method, mg_str ("POST")) == 0) {
                    handle_login(c, hm);
                } else {
                    mg_http_reply(c, HTTP_METHOD_NOT_ALLOWED, "",
                                  ERROR_METHOD_NOT_ALLOWED);
                }
                break;

            case ROUTE_DIV_SUBMIT:
                handle_get_div_check(c, hm);
                break;

            case ROUTE_DIV_FORM:
            case ROUTE_ROOT:
                handle_div_page(c);
                break;

            case ROUTE_UNKNOWN:
            default:
                mg_http_reply(c, HTTP_NOT_FOUND, "",
			      ERROR_NOT_FOUND);
                break;
            }
        }
        // Освобождение
        if (response) {
            free(response);
        }
    }
}

int main (void) {
    int ret = 0;   // флаг возвращаемого значения
                   //
    if (!getenv ("LOGIN_USER") || !getenv ("LOGIN_PASS")) {
        fprintf (stderr,
                 "Error: LOGIN_USER and LOGIN_PASS environment variables must be set\n");
        ret = 1;
    } else {
        struct mg_mgr mgr;
        mg_mgr_init(&mgr);
        if (mg_http_listen(&mgr, "http://127.0.0.1:8000", fn, NULL)) {
            printf ("Server running on http://127.0.0.1:8000\n");
            while (true) {
                mg_mgr_poll(&mgr, 1000);
            }
        } else {
            fprintf (stderr, "Failed to start server./n");
            ret = 1;
        }

        mg_mgr_free(&mgr);
    }

    return ret;
}
