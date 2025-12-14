#ifndef MULTITASKING_TASK_STATUSES_H
#define MULTITASKING_TASK_STATUSES_H

typedef enum {
    STATUS_IDLE = 0,      // нет задания
    STATUS_READY = 1,     // задание готово
    STATUS_DONE = 2       // результат готов
} TaskStatus;

#endif //MULTITASKING_TASK_STATUSES_H