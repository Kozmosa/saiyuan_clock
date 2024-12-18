//
// Created by 27866 on 2024/12/12.
//

#ifndef UTILS_H
#define UTILS_H

#define ALARM_DEFINED

typedef void (*void_callback_t)(void);

typedef struct {
    time_t alarm_timestamp;
    char alarm_time_string[32];
} alarm_t;

typedef struct {
  char (*time_s)[32];
  char (*date_s)[32];
  alarm_t* alarms;
  int* alarm_count;
  int* alarm_capacity;
} static_vars_t;

// extern void refresh_time(char *time_s, char *date_s);
// extern void get_time(char *time_a, char *date_a);
// extern void uart_app_main(void);

extern void refresh_time(char *time_s, char *date_s);
extern void get_time(char *time_a, char *date_a);
extern bool check_alarm(alarm_t* alarm);
extern void alarm_ring(void);
extern void check_alarms(static_vars_t* static_vars_container);
extern void alarm_set(static_vars_t* static_vars_container, int hh, int mm, int ss);
extern void button_app_main(void);
extern int button_key_check(void);
void echo_task(void *arg);
extern void alarm_task(void* pvParameters);
extern void uart_app_main(static_vars_t* static_vars_container);
extern void command_handler(char* command, static_vars_t* static_vars_container);

#endif //UTILS_H
