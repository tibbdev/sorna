/*
 * 
 *  file        :   sorna.h
 *  description :   sorna is a simple shell implementation.
 *  author      :   M. Tibbitts (github.com/tibbdev)
 *  date        :   22-07-2021 
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Constants
#define  SORNA_SUCCESS               ((int8_t)0)
#define  SORNA_FAIL                  ((int8_t)-1)
#define  SORNA_FAIL_UNKNOWN_CMD      ((int8_t)-2)
#define  SORNA_FAIL_UNKNOWN_ARG      ((int8_t)-3)

// Maximum number of arguments
#ifndef  SORNA_MAX_ARGS
#define  SORNA_MAX_ARGS  ((uint8_t)64)
#endif

// Maximum string length
#ifndef  SORNA_MAX_INPUT_STRING_LENGTH
#define  SORNA_MAX_INPUT_STRING_LENGTH  ((uint16_t)256)
#endif

// Maximum string length
#ifndef  SORNA_MAX_COMMANDS
#define  SORNA_MAX_COMMANDS ((uint8_t)16)
#endif

// Maximum string length
#ifndef  SORNA_MAX_PROMPT_LENGTH
#define  SORNA_MAX_PROMPT_LENGTH ((uint8_t)8)
#endif

// function typedefs
typedef int8_t  (*cmd_func_t)(uint8_t argc, char *argv[]);
typedef void    (*write_str_func_t)(char *str);

int8_t sorna_init(char * prompt, write_str_func_t write_str_f, char * welcome_msg);
int8_t sorna_tick(char ch);
int8_t sorna_register_cmd(char * cmd_str, cmd_func_t cmd_func, char *desc_short, char *desc_long);
void sorna_write_str(char *str);
