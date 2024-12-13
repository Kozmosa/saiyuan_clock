//
// Created by 27866 on 2024/12/12.
//

#ifndef UTILS_H
#define UTILS_H

typedef void (*void_callback_t)(void);
typedef struct {
    time_t alarm_timestamp;
    char alarm_time_string[32];
} alarm_t;

typedef struct {
  char (*time_s)[32];
  char (*date_s)[32];
  alarm_t* alarms;
  int alarm_count;
  int alarm_capacity;
} static_vars_t;

extern void refresh_time(char *time_s, char *date_s);
extern void get_time(char *time_a, char *date_a);
extern void uart_app_main(void);

#endif //UTILS_H
