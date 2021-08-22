#include "mezu.h"

typedef struct cmd_entry_struct
{
    char        *token;
    cmd_func_t  func;
    char        *desc_short;
    char        *desc_long;
} cmd_entry_t;

typedef struct mezu_struct
{
    bool                initialised;
    bool                echo;
    cmd_entry_t         commands[MEZU_MAX_COMMANDS];
    char                prompt[MEZU_MAX_PROMPT_LENGTH + 1];
    write_str_func_t    write_str_f;
    read_str_func_t     read_str_f;
    uint8_t             cmd_count;
    char                in_buff[MEZU_MAX_INPUT_STRING_LENGTH + 1];
    uint8_t             buff_pos;
} Mezu;

Mezu _mezu;

int8_t help_cmd(uint8_t argc, char *argv[]);
int8_t help_cmd(uint8_t argc, char *argv[])
{
    if(argc == 1)
    {
        _mezu.write_str_f("   MEZU HELP - all\r\n\r\n");
        for (uint8_t idx = 0; idx < _mezu.cmd_count; idx++)
        {
            _mezu.write_str_f("   ");
            _mezu.write_str_f(_mezu.commands[idx].token);
            _mezu.write_str_f(" : ");
            _mezu.write_str_f(_mezu.commands[idx].desc_short);
            _mezu.write_str_f("\r\n");
        }
        
        return MEZU_SUCCESS;
    }
    else if (argc > 1)
    {
        char *arg1 = argv[1];
        bool cmd_found = false;
        
        for (uint8_t idx = 0; ((idx < _mezu.cmd_count) && (!cmd_found)); idx++)
        {
            if (0 == strcmp(arg1, _mezu.commands[idx].token))
            {
                _mezu.write_str_f("   MEZU HELP - ");
                _mezu.write_str_f(_mezu.commands[idx].token);
                _mezu.write_str_f("\r\n\r\n");
                _mezu.write_str_f("   ");
                _mezu.write_str_f(_mezu.commands[idx].token);
                _mezu.write_str_f(" : ");
                _mezu.write_str_f(_mezu.commands[idx].desc_short);
                _mezu.write_str_f("\r\n");
                if(NULL != _mezu.commands[idx].desc_long)
                {
                    _mezu.write_str_f(_mezu.commands[idx].desc_long);
                    _mezu.write_str_f("\r\n");
                }
                cmd_found = true;
            }
        }
        if(cmd_found)
        {
            return MEZU_SUCCESS;
        }
        else
        {
            return MEZU_FAIL_UNKNOWN_CMD;
        }
        
    }
    return MEZU_FAIL;
}

int8_t mezu_init(char * prompt, write_str_func_t write_str_f, read_str_func_t read_str_f, char * welcome_msg)
{
    // clear arrays
    memset((void *)_mezu.prompt, 0, MEZU_MAX_PROMPT_LENGTH + 1);
    memset((void *)_mezu.in_buff, 0, MEZU_MAX_INPUT_STRING_LENGTH + 1);
    memset((void *)_mezu.commands, 0, sizeof(_mezu.commands));

    int8_t prompt_len = 0;
    int8_t rv = MEZU_SUCCESS;
    if(NULL == prompt)
    {
        strcpy(_mezu.prompt, "MEZU>");
    }
    else
    {
        prompt_len = strlen(prompt);
        int8_t n = prompt_len;
        if(MEZU_MAX_PROMPT_LENGTH < n)
        {
            n = MEZU_MAX_PROMPT_LENGTH;
        }
        strncpy(_mezu.prompt, prompt, prompt_len);
        if('>' != _mezu.prompt[prompt_len-1])
        {
            n--;
            strncpy(_mezu.prompt, prompt, n);
            strcat(_mezu.prompt, ">");
        }
    }

    _mezu.echo = true;
    _mezu.write_str_f = write_str_f;
    _mezu.read_str_f = read_str_f;
    _mezu.cmd_count = 0;
    _mezu.buff_pos = 0;

    rv = mezu_register_cmd("help", help_cmd, "provides help for registered commands", 
            "   usage : help <arg1>\r\n"
            "     NOTE: arg1 is optional\r\n"
            "     arg1 : name of the command for more detailed instructions\r\n"
            "     with no value for arg1, short descriptions of all registered functions will be displayed\r\n");

    _mezu.initialised = true;

    if(NULL != welcome_msg)
    {
        mezu_write_str(welcome_msg);
        mezu_write_str("\r\n.\r\n\r\n");
    }
    else
    {
        _mezu.write_str_f("\r\n   ***   MEZU   ***\r\n\r\n");
    }

    mezu_write_str(_mezu.prompt);

    return rv;
}

int8_t mezu_tick(char ch)
{
    if(_mezu.initialised)
    {
        // Read Input character
        if (MEZU_MAX_INPUT_STRING_LENGTH > _mezu.buff_pos)
        {
            if ('\b' == ch)
            {
                if(0 < _mezu.buff_pos)
                {
                    _mezu.buff_pos--;
                    _mezu.in_buff[_mezu.buff_pos] = '\0';
                }
            }
            else
            {
                _mezu.in_buff[_mezu.buff_pos] = ch;
            }
            if(_mezu.echo)
            {
                char tx[2] = {ch, 0};
                mezu_write_str(tx);
            }
        }
        else
        {
            mezu_write_str("\r\nERR:mezu-Input_Buffer_Full\r\n");
            return MEZU_FAIL;
        }
        
        // if Carriage return of New line character is latest char received, process input buffer, execute command if possible, then clear input buffer
        // If input buffer is not empty
        if(NULL != _mezu.in_buff)
        {
            // if carriage return or new line are the last characters in the buffer
            if(('\r' != _mezu.in_buff[_mezu.buff_pos - 1]) || ('\n' != _mezu.in_buff[_mezu.buff_pos - 1]))
            {
                uint8_t argc = 0;
                char *argv[MEZU_MAX_ARGS];
                bool no_more_args = false;

                for (uint8_t idx = 0; ((idx < MEZU_MAX_ARGS) && !no_more_args); idx++)
                {
                    argv[idx] = strtok(_mezu.in_buff, " ");
                    if(NULL == argv[idx])
                    {
                        no_more_args = true;
                    }
                    argc++;
                }

                if(!no_more_args)
                {
                    mezu_write_str("\r\nWNG:mezu-More_Arguments_Than_Supported\r\n");
                }

                char * token = argv[0];
                if(token == NULL)
                {
                    _mezu.in_buff[0] = '\0';
                    _mezu.buff_pos = 0;
                    mezu_write_str("ERR:mezu-Command_Token_Cannot_Be_NULL\r\n");
                    return MEZU_FAIL;
                }
                for (uint8_t idx = 0; idx < _mezu.cmd_count; idx++)
                {
                    if(0 == strcmp(token, _mezu.commands[idx].token))
                    {
                        int8_t rv = _mezu.commands[idx].func(argc, argv);
                        if(MEZU_FAIL_UNKNOWN_ARG == rv)
                        {
                            mezu_write_str("\r\nWNG:");
                            mezu_write_str(token);
                            mezu_write_str("-Unknown_Argument_Failure\r\n");
                        }
                        return rv;
                    }
                }
                mezu_write_str("\r\nWNG:");
                mezu_write_str(token);
                mezu_write_str("-Unknown_Command_Failure\r\n");
                return MEZU_FAIL_UNKNOWN_CMD;
                
            }
        }
    }
    else
    {
        return MEZU_FAIL;
    }
}
int8_t mezu_register_cmd(char * cmd_str, cmd_func_t cmd_func, char *desc_short, char *desc_long)
{
    // Check for valid details
    if(NULL == cmd_str)
    {
        if (NULL != _mezu.write_str_f)
        {
            _mezu.write_str_f("ERR:mezu-Command_Token_Cannot_Be_NULL\r\n");
        }
        
        return MEZU_FAIL;
    }
    else
    {
        for (uint8_t idx = 0; idx < _mezu.cmd_count; idx++)
        {
            if(0 == strcmp(_mezu.commands[idx].token, cmd_str))
            {
                if (NULL != _mezu.write_str_f)
                {
                    _mezu.write_str_f("ERR:mezu-Cannot_Have_Duplicate_Command_Tokens\r\n");
                }
                return MEZU_FAIL;
            }
        }
    }
    if(NULL == cmd_func)
    {
        if (NULL != _mezu.write_str_f)
        {
            _mezu.write_str_f("ERR:mezu-Command_Function_Cannot_Be_NULL\r\n");
        }
        return MEZU_FAIL;
    }
    if(NULL == desc_short)
    {
        if (NULL != _mezu.write_str_f)
        {
            _mezu.write_str_f("ERR:mezu-Short_Description_Cannot_Be_NULL\r\n");
        }
        return MEZU_FAIL;
    }
#ifdef MEZU_COMPULSARY_LONG_DESC
    if(NULL == desc_long)
    {
        if (NULL != _mezu.write_str_f)
        {
            _mezu.write_str_f("ERR:mezu-Long_Description_Cannot_Be_NULL\r\n");
        }
        return MEZU_FAIL;
    }
#endif
    if(MEZU_MAX_COMMANDS > _mezu.cmd_count)
    {
        _mezu.commands[_mezu.cmd_count].token       = cmd_str;
        _mezu.commands[_mezu.cmd_count].func        = cmd_func;
        _mezu.commands[_mezu.cmd_count].desc_short  = desc_short;
        _mezu.commands[_mezu.cmd_count].desc_long   = desc_long;
        return MEZU_SUCCESS;
    }
    else
    {
        mezu_write_str("ERR:mezu-Too_Many_Commands_Registered\r\n");
        return MEZU_FAIL;
    }
    
    return MEZU_FAIL;
}


void mezu_write_str(char *str)
{
    if (NULL != _mezu.write_str_f)
    {
        _mezu.write_str_f(str);
    }
}
