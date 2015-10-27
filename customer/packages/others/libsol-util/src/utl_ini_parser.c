/***********************************************************************
 *
 *  Copyright (c) 2015 sphantix
 *  All Rights Reserved
 *
# 
# 
# 
#
 * 
 ************************************************************************/

#include <ctype.h>
#include "utl_ini_parser.h"
#include "utl_memory.h"
#include "utl_strconv.h"
#include "utl_logging.h"

static UBOOL8 utl_ini_check_value(char *group, char *key, char *value)
{
    return((utlStr_strlen(group) > 0) && (utlStr_strlen(key) > 0) && (utlStr_strlen(value) > 0));
}

UBOOL8 utl_ini_parse_buffer(char* buffer, char comment_char, char delim_char, utl_ini_handler handler, void *cxdata)
{
    char *p = buffer;
    char *group = NULL;
    char *key = NULL;
    char *value = NULL;

    enum _State
    {
        STAT_NONE = 0,
        STAT_GROUP,
        STAT_KEY,
        STAT_VALUE,
        STAT_COMMENT
    }state = STAT_NONE;

    for(p = buffer; *p != '\0'; p++)
    {
        switch(state)
        {
            case STAT_NONE:
                {
                    if(*p == '[')
                    {
                        state = STAT_GROUP;
                        group = p + 1;
                    }
                    else if(*p == comment_char)
                    {
                        state = STAT_COMMENT;
                    }
                    else if(!isspace(*p))
                    {
                        state = STAT_KEY;
                        key = p;
                    }
                    else if(*p == delim_char)
                        goto error;
                    break;
                }
            case STAT_GROUP:
                {
                    if(*p == ']')
                    {
                        *p = '\0';
                        state = STAT_NONE;
                        utlStr_strTrim(group);
                    }
                    else if (*p == delim_char) 
                        goto error;
                    break;
                }
            case STAT_COMMENT:
                {
                    if(*p == '\n')
                    {
                        state = STAT_NONE;
                        break;
                    }
                    break;
                }
            case STAT_KEY:
                {
                    if(*p == delim_char || (delim_char == ' ' && *p == '\t'))
                    {
                        *p = '\0';
                        state = STAT_VALUE;
                        value = p + 1;
                    }
                    break;
                }
            case STAT_VALUE:
                {
                    if(*p == '\n' || *p == '\r')
                    {
                        *p = '\0';
                        state = STAT_NONE;
                        utlStr_strTrim(key);
                        utlStr_strTrim(value);
                        if (utl_ini_check_value(group, key, value)) 
                        {
                            if (!handler(group, key, value, cxdata)) 
                                goto error;
                        }
                        else
                            goto error;
                    }
                    break;
                }
            default:break;
        }
    }

    if(state == STAT_VALUE)
    {
        utlStr_strTrim(key);
        utlStr_strTrim(value);
        if (utl_ini_check_value(group, key, value)) 
        {
            if (!handler(group, key, value, cxdata)) 
                goto error;
        }
        else
            goto error;
    }

    return TRUE;

error:
    utlLog_error("ini config file parse error, please check config file!");
    return FALSE;
}

UBOOL8 utl_ini_parse_file(const char *filename, char comment_char, char delim_char, utl_ini_handler handler, void *cxdata)
{
    FILE *fp;
    int len = 0;
    char *buff = NULL;
    UBOOL8 ret = FALSE;

    if((fp = fopen(filename, "r")) == NULL)
        return FALSE;
    
    fseek(fp, 0 , SEEK_END);
    len = ftell(fp);
    rewind(fp);
    buff = (char *)utlMem_alloc(sizeof(char)*len + 1, ALLOC_ZEROIZE);
    fread(buff, sizeof(char), len+1, fp);
    fclose(fp);

    ret = utl_ini_parse_buffer(buff, comment_char, delim_char, handler, cxdata);
    utlMem_free(buff);

    return ret;
}
