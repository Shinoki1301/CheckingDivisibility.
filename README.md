# ЛАБОРАТОРНАЯ РАБОТА №II.3. Разработка простого UI-веб-приложения на языке Си с использованием библиотеки Mongoose 

# Оглавление

- [Вступление](#описание-проекта)
    - [Цель проекта](#цель-работы)
    - [Задачи проекта](#основные-задачи)
    - [Структура проекта](#структура-проекта)
- [Сборка и запуск](#сборка-и-запуск-проекта)
- [Задание на защиту](#задание-на-защиту)
- [Вывод](#вывод)


   
## Описание проекта

Данный проект представляет собой реализацию простого UI-веб-приложение на языке программирования Си с использованием легковесной библиотеки Mongoose, предназначенной для обработки HTTP-запросов. В рамках проекта создается минималистичное веб-приложение, с поддержкой авторизации пользователя через HTML-форму. Веб-интерфейс использует встроенные статические ресурсы (HTML и CSS) и взаимодуйствует с пользователем посредством GET- и POST-запросов. Проект служит основой для изучения принципов построения UI-веб-приложений средствами языка C и библиотек низкого уровня, а также для приобретения практических навыков работы с сетевыми приложениями и веб-технологиями.


### Цель работы:

Изучить принципы создания простых веб-интерфейсов с использованием языка программирования C и библиотеки Mongoose; освоить обработку HTTP-запросов, передачу данных через HTML-формы и реализацию базовой логики взаимодействия с пользователем; получить навыки базовой работы с HTML/CSS в составе UI-приложения, обеспечить запуск и сопровождение веб-приложения в среде ОС Linux.


### Основные задачи:

1. Ознакомление с основами разработки UI-приложений на языке Си с использованием библиотеки Mongoose.
2. Изучение принципов обработки HTTP-запросов (GET и POST) и организации маршрутизации внутри приложения.
3. Реализация формы авторизации пользователя с получением и анализом данных, переданных из HTML-формы.
4. Подключение и использование статических ресурсов (HTML, CSS) в составе Cпрограммы.
5. Настройка автоматизированной сборки проекта с помощью Makefile.
6. Работа с переменными окружения для хранения конфиденциальных данных (имя пользователя и пароль).
7. Анализ поведения приложения при различных типах пользовательского ввода и условий обработки (успешная авторизация, неправильный ввод, отсутствие переменных среды).
8. Реализация базовой логики веб-приложения с возвратом соответствующих страниц и кодов ответа HTTP.
9. Проверка работоспособности приложения в среде ОС Linux с использованием браузера и утилит командной строки (например, curl или wget).
10. Выполнение индивидуального задания.
11. Подготовить документацию к проекту и выложить проект на удаленный git-репозиторий.


### Структура проекта
```
/CheckingDivisibility.
├── constants
│   ├── constants.h
│   ├── error_messages.h
│   ├── errors.html
│   └── http_status.h
├── css
│   └── style.css
├── input
│   ├── input.c
│   └── input.h
├── mongoose
│   ├── mongoose.c
│   └── mongoose.h
├── router
│   ├── router.c
│   └── router.h
├── templase
│   ├── divisibility.html
│   ├── error.html
│   └── login.html
│── .clang-format       
│── .gitignore       
│── Makefile
├── README.md
│── main.c
├── run.sh
```


## Сборка и запуск проекта
**Для сборки программы:**
```sh
make
```

**Для запуска сервера:**
```sh
make run
```

**Для очистки проекта:**
```sh
make clean
```

## Задание на защиту
**Условие:** Необходимо изменить часть кода, который обрабатывает событие, сделав код читаемым.
**Код обработчика событий до изменений:**
```sh
// Обработчик событий
static void fn (struct mg_connection* c, int ev, void* ev_data) {
    if (ev != MG_EV_HTTP_MSG) return;

    struct mg_http_message* hm = (struct mg_http_message*) ev_data;

    if (mg_strcmp (hm->uri, mg_str (ROUTE_STYLES)) == 0) {
        handle_css_request (c);
    } else if (mg_strcmp (hm->uri, mg_str (ROUTE_LOGOUT)) == 0) {
        handle_logout (c);
    } else if (mg_strcmp (hm->uri, mg_str (ROUTE_LOGIN)) == 0 &&
               mg_strcmp (hm->method, mg_str ("POST")) == 0) {
        handle_login (c, hm);
    } else if (!authenticated) {
        // Неавторизован — показать форму логина
        char* response = read_file (PATH_LOGIN_HTML);
        mg_http_reply (c, 200, CONTENT_TYPE_HTML, "%s", response);
        free (response);
    } else if (mg_strcmp (hm->uri, mg_str (ROUTE_DIV_SUBMIT)) == 0) {
        handle_get_div_check (c, hm);
    } else if (mg_strcmp (hm->uri, mg_str (ROUTE_DIV_FORM)) == 0 ||
               mg_strcmp (hm->uri, mg_str ("/")) == 0 ||
               mg_strcmp (hm->uri, mg_str ("/div")) == 0) {
        handle_div_page (c);
    } else {
        mg_http_reply (c, 404, "", "Not Found");
    }
}
```
---
**Код обработчика событий после изменений:**
```sh
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
```
Для изменения кода были созданы файлы router.c и router.h, в которых реализована таблица HTTP-маршрутов.


## Вывод
В результате работы был разработан простой UI-веб-сервер на языке программирования C с использованием библиотеки Mongoose. Были изучены ключевые аспекты создания веб-приложений на низкоуровневом языке.


