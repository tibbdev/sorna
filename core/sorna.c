/*
 *
 *  file        :   sorna.c
 *  description :   sorna is a simple shell implementation.
 *  author      :   M. Tibbitts (github.com/tibbdev)
 *  date        :   22-07-2021
 *
 */
#include "sorna.h"

typedef struct cmd_entry_struct
{
    char        *token;
    cmd_func_t  func;
    char        *desc_short;
    char        *desc_long;
} cmd_entry_t;

typedef struct sorna_struct
{
    bool                initialised;
    bool                echo;
    cmd_entry_t         commands[SORNA_MAX_COMMANDS];
    char                prompt[SORNA_MAX_PROMPT_LENGTH + 1];
    write_str_func_t    write_str_f;
    reset_func_t        reset_f;
    uint8_t             cmd_count;
    char                in_buff[SORNA_MAX_INPUT_STRING_LENGTH + 1];
    uint8_t             buff_pos;
} Sorna;

Sorna _sorna;

int8_t help_cmd(uint8_t argc, char *argv[]);
int8_t help_cmd(uint8_t argc, char *argv[])
{
    if(argc == 1)
    {
        _sorna.write_str_f("   SORNA HELP - all\r\n\r\n");
        for (uint8_t idx = 0; idx < _sorna.cmd_count; idx++)
        {
            _sorna.write_str_f("   ");
            _sorna.write_str_f(_sorna.commands[idx].token);
            _sorna.write_str_f(" : ");
            _sorna.write_str_f(_sorna.commands[idx].desc_short);
            _sorna.write_str_f("\r\n");
        }
        
        return SORNA_SUCCESS;
    }
    else if (argc > 1)
    {
        char *arg1 = argv[1];
        bool cmd_found = false;
        
        for (uint8_t idx = 0; ((idx < _sorna.cmd_count) && (!cmd_found)); idx++)
        {
            if (0 == strcmp(arg1, _sorna.commands[idx].token))
            {
                _sorna.write_str_f("   SORNA HELP - ");
                _sorna.write_str_f(_sorna.commands[idx].token);
                _sorna.write_str_f("\r\n\r\n");
                _sorna.write_str_f("   ");
                _sorna.write_str_f(_sorna.commands[idx].token);
                _sorna.write_str_f(" : ");
                _sorna.write_str_f(_sorna.commands[idx].desc_short);
                _sorna.write_str_f("\r\n");
                if(NULL != _sorna.commands[idx].desc_long)
                {
                    _sorna.write_str_f(_sorna.commands[idx].desc_long);
                    _sorna.write_str_f("\r\n");
                }
                cmd_found = true;
            }
        }
        if(cmd_found)
        {
            return SORNA_SUCCESS;
        }

        return SORNA_FAIL_UNKNOWN_CMD;
        
    }
    return SORNA_FAIL;
}

int8_t reset_cmd(uint8_t argc, char *argv[]);
int8_t reset_cmd(uint8_t argc, char *argv[])
{
    sorna_write_str("\r\n...Resetting");
    _sorna.reset_f();

    sorna_write_str("\r\nERR:reset-DIDNT_RESET!");
    return -1;
}

int8_t sorna_init(char * prompt, write_str_func_t write_str_f, reset_func_t reset_f, char * welcome_msg)
{
    // clear arrays
    memset((void *)_sorna.prompt, 0, SORNA_MAX_PROMPT_LENGTH + 1);
    memset((void *)_sorna.in_buff, 0, SORNA_MAX_INPUT_STRING_LENGTH + 1);
    memset((void *)_sorna.commands, 0, sizeof(_sorna.commands));

    int8_t prompt_len = 0;
    int8_t rv = SORNA_SUCCESS;
    if(NULL == prompt)
    {
        strcpy(_sorna.prompt, "SORNA>");
    }
    else
    {
        prompt_len = strlen(prompt);
        int8_t n = prompt_len;
        if(SORNA_MAX_PROMPT_LENGTH < n)
        {
            n = SORNA_MAX_PROMPT_LENGTH;
        }
        strncpy(_sorna.prompt, prompt, prompt_len);
        if('>' != _sorna.prompt[prompt_len-1])
        {
            n--;
            strncpy(_sorna.prompt, prompt, n);
            strcat(_sorna.prompt, ">");
        }
    }

    _sorna.echo = true;
    _sorna.write_str_f = write_str_f;
    _sorna.cmd_count = 0;
    _sorna.buff_pos = 0;

    rv = sorna_register_cmd("help", help_cmd, "provides help for registered commands",
            "   usage : help <arg1>\r\n"
            "     NOTE: arg1 is optional\r\n"
            "     arg1 : name of the command for more detailed instructions\r\n"
            "     with no value for arg1, short descriptions of all registered functions will be displayed\r\n");

    if(NULL != reset_f)
    {
        _sorna.reset_f = reset_f;
        rv = sorna_register_cmd("reset", reset_cmd, "performs software reset",
            "   usage : reset\r\n");
    }

    _sorna.initialised = true;

    if(NULL != welcome_msg)
    {
        sorna_write_str(welcome_msg);
        sorna_write_str("\r\n");
    }
    else
    {
        _sorna.write_str_f("\r\n   ***   SORNA   ***\r\n\r\n");
    }

    sorna_write_str(_sorna.prompt);

    return rv;
}

int8_t sorna_tick(char ch)
{
    if(_sorna.initialised)
    {
        if(_sorna.echo)
        {
            char tx[2] = {ch, 0};
            sorna_write_str(tx);
        }

        if(0u == ch)
        {
            return SORNA_FAIL;
        }

        // Read Input character
        if (SORNA_MAX_INPUT_STRING_LENGTH > _sorna.buff_pos)
        {
            if (('\b' == ch) || ((char)0x48 == ch))
            {
                if(0 < _sorna.buff_pos)
                {
                    _sorna.buff_pos--;
                    _sorna.in_buff[_sorna.buff_pos] = '\0';
                }
            }
            else
            {
                _sorna.in_buff[_sorna.buff_pos] = ch;
                _sorna.buff_pos++;
            }
        }
        else
        {
            sorna_write_str("\r\nERR:sorna-Input_Buffer_Full\r\n");
            return SORNA_FAIL;
        }
        
        // if Carriage return of New line character is latest char received, process input buffer, execute command if possible, then clear input buffer
        // If input buffer is not empty
        if(NULL != _sorna.in_buff)
        {
            // if carriage return or new line are the last characters in the buffer
            if(('\r' == _sorna.in_buff[_sorna.buff_pos - 1]) || ('\n' == _sorna.in_buff[_sorna.buff_pos - 1]))
            {
                // erase last char from buffer
                _sorna.buff_pos--;
                _sorna.in_buff[_sorna.buff_pos] = '\0';

                if(!strlen(_sorna.in_buff))
                {
                    return SORNA_SUCCESS;
                }

                // get ready...
                uint8_t argc = 0;
                char *argv[SORNA_MAX_ARGS];
                bool no_more_args = false;

                argv[0] = strtok(_sorna.in_buff, " ");
                for (uint8_t idx = 1; ((idx < SORNA_MAX_ARGS) && !no_more_args); idx++)
                {
                    argv[idx] = strtok(NULL, " ");
                    if(NULL == argv[idx])
                    {
                        no_more_args = true;
                    }
                    argc++;
                }

                // check what was processed...
                if(!no_more_args)
                {
                    sorna_write_str("\r\nWNG:sorna-More_Arguments_Than_Supported\r\n");
                }

                if(argv[0] == NULL)
                {
                    _sorna.in_buff[0] = '\0';
                    _sorna.buff_pos = 0;
                    sorna_write_str("ERR:sorna-Command_Token_Cannot_Be_NULL\r\n");
                    return SORNA_FAIL;
                }

                for (uint8_t idx = 0; idx < _sorna.cmd_count; idx++)
                {
                    if(0 == strcmp(argv[0], _sorna.commands[idx].token))
                    {
                        int8_t rv = _sorna.commands[idx].func(argc, argv);
                        if(SORNA_FAIL_UNKNOWN_ARG == rv)
                        {
                            sorna_write_str("\r\nWNG:");
                            sorna_write_str(argv[0]);
                            sorna_write_str("-Unknown_Argument_Failure\r\n");
                        }

                        // reset input buffer now it has finished being processed...
                        _sorna.in_buff[0] = '\0';
                        _sorna.buff_pos = 0;

                        sorna_write_str(_sorna.prompt);
                        return rv;
                    }
                }

                // Command wasn't found, display warning message...
                sorna_write_str("\r\nWNG:");
                sorna_write_str(argv[0]);
                sorna_write_str("-Unknown_Command_Failure\r\n");

                // reset input buffer now it has finished being processed...
                _sorna.in_buff[0] = '\0';
                _sorna.buff_pos = 0;
                sorna_write_str(_sorna.prompt);
                return SORNA_FAIL_UNKNOWN_CMD;
                
            }

            return SORNA_SUCCESS;
        }
    }

    return SORNA_FAIL;
}
int8_t sorna_register_cmd(char * cmd_str, cmd_func_t cmd_func, char *desc_short, char *desc_long)
{
    // Check for valid details
    if(NULL == cmd_str)
    {
        if (NULL != _sorna.write_str_f)
        {
            _sorna.write_str_f("ERR:sorna-Command_Token_Cannot_Be_NULL\r\n");
        }
        
        return SORNA_FAIL;
    }
    else
    {
        for (uint8_t idx = 0; idx < _sorna.cmd_count; idx++)
        {
            if(0 == strcmp(_sorna.commands[idx].token, cmd_str))
            {
                if (NULL != _sorna.write_str_f)
                {
                    _sorna.write_str_f("ERR:sorna-Cannot_Have_Duplicate_Command_Tokens\r\n");
                }
                return SORNA_FAIL;
            }
        }
    }
    if(NULL == cmd_func)
    {
        if (NULL != _sorna.write_str_f)
        {
            _sorna.write_str_f("ERR:sorna-Command_Function_Cannot_Be_NULL\r\n");
        }
        return SORNA_FAIL;
    }
    if(NULL == desc_short)
    {
        if (NULL != _sorna.write_str_f)
        {
            _sorna.write_str_f("ERR:sorna-Short_Description_Cannot_Be_NULL\r\n");
        }
        return SORNA_FAIL;
    }
#ifdef SORNA_COMPULSARY_LONG_DESC
    if(NULL == desc_long)
    {
        if (NULL != _sorna.write_str_f)
        {
            _sorna.write_str_f("ERR:sorna-Long_Description_Cannot_Be_NULL\r\n");
        }
        return SORNA_FAIL;
    }
#endif
    if(SORNA_MAX_COMMANDS > _sorna.cmd_count)
    {
        _sorna.commands[_sorna.cmd_count].token       = cmd_str;
        _sorna.commands[_sorna.cmd_count].func        = cmd_func;
        _sorna.commands[_sorna.cmd_count].desc_short  = desc_short;
        _sorna.commands[_sorna.cmd_count].desc_long   = desc_long;
        _sorna.cmd_count++;
        return SORNA_SUCCESS;
    }
    else
    {
        sorna_write_str("ERR:sorna-Too_Many_Commands_Registered\r\n");
        return SORNA_FAIL;
    }
    
    return SORNA_FAIL;
}

void sorna_write_str(char const * const str)
{
    if (NULL != _sorna.write_str_f)
    {
        _sorna.write_str_f(str);
    }
}
