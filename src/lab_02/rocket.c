#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h> // Для базовой работы с X11
#include <X11/Xos.h>


void draw_rocket(Display *display, Window window, GC gc, int screen) {
    // Чёрный фон
    XSetForeground(display, gc, BlackPixel(display, screen));
    XFillRectangle(display, window, gc, 0, 0, 800, 600);

    // Звёзды
    XSetForeground(display, gc, WhitePixel(display, screen));
    for (int i = 0; i < 100; i++) {
        int x = rand() % 800;
        int y = rand() % 600;
        XFillRectangle(display, window, gc, x, y, 2, 2);
    }

    // Координаты центра ракеты
    int rocket_x = 400;
    int rocket_y = 300;

    // Корпус ракеты (серый прямоугольник)
    XSetForeground(display, gc, 0xAAAAAA);  // Серый цвет
    XFillRectangle(display, window, gc, rocket_x - 20, rocket_y, 40, 120);

    // Нос ракеты (красный треугольник)
    XSetForeground(display, gc, 0xFF0000);  // Красный
    XPoint nose[3] = {
        {rocket_x - 20, rocket_y},      // Левый угол
        {rocket_x + 20, rocket_y},      // Правый угол
        {rocket_x, rocket_y - 40}       // Верхняя точка
    };
    XFillPolygon(display, window, gc, nose, 3, Convex, CoordModeOrigin);

    // Иллюминаторы (синие круги)
    XSetForeground(display, gc, 0x0000FF);  // Синий
    XFillArc(display, window, gc, rocket_x - 10, rocket_y + 20, 20, 20, 0, 360 * 64);
    XFillArc(display, window, gc, rocket_x - 10, rocket_y + 60, 20, 20, 0, 360 * 64);

    // Левый стабилизатор (жёлтый треугольник)
    XSetForeground(display, gc, 0xFFFF00);  // Жёлтый
    XPoint left_fin[3] = {
        {rocket_x - 20, rocket_y + 100},
        {rocket_x - 20, rocket_y + 120},
        {rocket_x - 40, rocket_y + 120}
    };
    XFillPolygon(display, window, gc, left_fin, 3, Convex, CoordModeOrigin);

    // Правый стабилизатор (жёлтый треугольник)
    XPoint right_fin[3] = {
        {rocket_x + 20, rocket_y + 100},
        {rocket_x + 20, rocket_y + 120},
        {rocket_x + 40, rocket_y + 120}
    };
    XFillPolygon(display, window, gc, right_fin, 3, Convex, CoordModeOrigin);

    // Пламя из двигателя (оранжевый + жёлтый)
    XSetForeground(display, gc, 0xFF6600);  // Оранжевый
    XPoint flame1[3] = {
        {rocket_x - 20, rocket_y + 120},
        {rocket_x, rocket_y + 160},
        {rocket_x - 10, rocket_y + 120}
    };
    XFillPolygon(display, window, gc, flame1, 3, Convex, CoordModeOrigin);

    XPoint flame2[3] = {
        {rocket_x + 20, rocket_y + 120},
        {rocket_x, rocket_y + 160},
        {rocket_x + 10, rocket_y + 120}
    };
    XFillPolygon(display, window, gc, flame2, 3, Convex, CoordModeOrigin);

    XSetForeground(display, gc, 0xFFFF00);  // Жёлтый центр пламени
    XPoint flame3[3] = {
        {rocket_x - 10, rocket_y + 120},
        {rocket_x, rocket_y + 150},
        {rocket_x + 10, rocket_y + 120}
    };
    XFillPolygon(display, window, gc, flame3, 3, Convex, CoordModeOrigin);
}


int main(void) {
    // 1. Установка соединения с X-сервером
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Не удалось открыть дисплей\n");
        return 1;
    }

    // 2. Инициализация графической подсистемы
    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    // 3. Создание и отображение окна
    Window window = XCreateSimpleWindow(
        display, root,
        100, 100,                     // x, y позиция
        800, 600,                     // ширина, высота
        1,                            // ширина рамки
        BlackPixel(display, screen),  // цвет рамки
        BlackPixel(display, screen)   // цвет фона
    );

    // Установка заголовка окна
    XStoreName(display, window, "Rocket in Space");

    // Выбор событий
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    // Отображение окна
    XMapWindow(display, window);

    // Создание графического контекста
    GC gc = XCreateGC(display, window, 0, NULL);

    // 4. Цикл обработки событий
    XEvent event;
    int running = 1;

    while (running) {
        XNextEvent(display, &event);

        switch (event.type) {
            case Expose:
                // Перерисовка при открытии/изменении окна
                if (event.xexpose.count == 0) {
                    draw_rocket(display, window, gc, screen);
                }
                break;

            case KeyPress:
                // 5. Выход при нажатии любой клавиши
                running = 0;
                break;
        }
    }

    // 6. Закрытие окна и разрыв соединения
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}