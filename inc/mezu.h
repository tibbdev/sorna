#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Constants
#define  MEZU_SUCCESS               ((int8_t)0)
#define  MEZU_FAIL                  ((int8_t)-1)
#define  MEZU_FAIL_UNKNOWN_CMD      ((int8_t)-2)
#define  MEZU_FAIL_UNKNOWN_ARG      ((int8_t)-3)

// Maximum number of arguments
#ifndef  MEZU_MAX_ARGS
#define  MEZU_MAX_ARGS  ((uint8_t)64)
#endif

// Maximum string length
#ifndef  MEZU_MAX_INPUT_STRING_LENGTH
#define  MEZU_MAX_INPUT_STRING_LENGTH  ((uint16_t)256)
#endif

// Maximum string length
#ifndef  MEZU_MAX_COMMANDS
#define  MEZU_MAX_COMMANDS ((uint8_t)16)
#endif

// Maximum string length
#ifndef  MEZU_MAX_PROMPT_LENGTH
#define  MEZU_MAX_PROMPT_LENGTH ((uint8_t)8)
#endif

// function typedefs
typedef int8_t (*cmd_func_t)(uint8_t argc, char *argv[]);
typedef void    (*write_str_func_t)(char *str);
typedef char *  (*read_str_func_t)();

int8_t mezu_init(char * prompt, write_str_func_t write_str_f, read_str_func_t read_str_f, char * welcome_msg);
int8_t mezu_tick(char ch);
int8_t mezu_register_cmd(char * cmd_str, cmd_func_t cmd_func, char *desc_short, char *desc_long);
void mezu_write_str(char *str);
