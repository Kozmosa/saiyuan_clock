//
// Created by 27866 on 2024/12/12.
//

#ifndef UTILS_H
#define UTILS_H

typedef void (*void_callback_t)(void);
typedef struct {
  char* time_s;
  char* date_s;
} static_vars_t;

extern void refresh_time(char *time_s, char *date_s);
extern void get_time(char *time_a, char *date_a);

#endif //UTILS_H
