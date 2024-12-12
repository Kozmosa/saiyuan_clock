#ifndef ACTIVITY_H
#define ACTIVITY_H
typedef struct {
    TaskHandle_t* task_handle_p;
    char* task_name;
    int last_time;
    void_callback_t* destroyed_callback;
} activity_args_t;

extern void activity_alarm_setting(activity_args_t* args);
extern void activity_alarm_ringtone_setting(activity_args_t* args);
extern void activity_alarm_ringing(activity_args_t* args);
extern void activity_clock_main(activity_args_t* args);
#endif