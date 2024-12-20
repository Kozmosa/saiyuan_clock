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
  char command_str[128];
  int length;
} command_t;

typedef struct {
  char (*time_s)[32];
  char (*date_s)[32];
  alarm_t* alarms;
  int* alarm_count;
  int* alarm_capacity;
  command_t* (*commands)[8];
  command_t* command_last;
  bool* isCommand;
} static_vars_t;


// extern void refresh_time(char *time_s, char *date_s);
// extern void get_time(char *time_a, char *date_a);
// extern void uart_app_main(void);

enum BUTTON_KEY_FUNCTIONS {
    BUTTON_KEY_MAIN_ACTIVITY = 0,
    BUTTON_KEY_ALARM_ACTIVITY,
    BUTTON_KEY_ALARM_RINGTONE_ACTIVITY,
    BUTTON_KEY_ALARM_SETTING_ACTIVITY,
    BUTTON_KEY_CLICK_UP,
    BUTTON_KEY_CLICK_DOWN,
    BUTTON_KEY_CLICK_LEFT,
    BUTTON_KEY_CLICK_RIGHT,
    BUTTON_KEY_CONFIRM,
    BUTTON_KEY_CANCEL,
    BUTTON_KEY_RESET,
    BUTTON_KEY_BLANK
};

extern void refresh_time(char *time_s, char *date_s);
extern void get_time(char *time_a, char *date_a);
extern void convert_time_to_hhmmss(time_t time, int* hh, int* mm, int* ss);
extern time_t convert_hhmmss_to_timestamp(int hh, int mm, int ss);
extern bool check_alarm(alarm_t* alarm);
extern void alarm_ring(alarm_t* alarm, static_vars_t* static_vars_container);
extern void check_alarms(static_vars_t* static_vars_container);
extern void alarm_set(static_vars_t* static_vars_container, int hh, int mm, int ss);
extern void button_app_main(void);
extern int button_key_check(void);
void echo_task(void *arg);
extern void alarm_task(void* pvParameters);
extern void alarm_app_main(static_vars_t* static_vars_container);
extern void uart_app_main(static_vars_t* static_vars_container);
extern void command_handler(char* command, static_vars_t* static_vars_container);

#endif //UTILS_H
