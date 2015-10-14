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

DLIST_HEAD(ini_group_head);

static void utl_ini_init_group_node(UtlIniGroup *node)
{
    node->pair_num = 0;
    INIT_DLIST_HEAD(&node->pair_head);
    dlist_add(&node->group_node, &ini_group_head);
}

static UBOOL8 utl_ini_check_value(char *key, char *value)
{
    return((utlStr_strlen(key) > 0) && (utlStr_strlen(value) > 0));
}

static void utl_ini_cleanup(DlistNode *head)
{
    UtlIniGroup *group_ptr = NULL;
    UtlIniGroup *group_next = NULL;
    UtlIniValuePair *pair_ptr = NULL;
    UtlIniValuePair *pair_next = NULL;

    dlist_for_each_entry_safe(group_ptr, group_next, &ini_group_head, group_node)
    {
        dlist_for_each_entry_safe(pair_ptr, pair_next, &group_ptr->pair_head, pair_node)
        {
            dlist_del(&pair_ptr->pair_node);
            UTLMEM_FREE_BUF_AND_NULL_PTR(pair_ptr->key);
            UTLMEM_FREE_BUF_AND_NULL_PTR(pair_ptr->value);
            UTLMEM_FREE_BUF_AND_NULL_PTR(pair_ptr);
            group_ptr->pair_num--;
        }
        
        if(dlist_empty(&group_ptr->pair_head) && group_ptr->pair_num == 0)
        {
            dlist_del(&group_ptr->group_node);
            UTLMEM_FREE_BUF_AND_NULL_PTR(group_ptr->group_name);
            UTLMEM_FREE_BUF_AND_NULL_PTR(group_ptr);
        }
    }
}

UBOOL8 utl_ini_parser(char* buffer, char comment_char, char delim_char, UtlIniValueHandler func)
{
    char *p = buffer;
    char *group_start = NULL;
    char *key_start   = NULL;
    char *value_start = NULL;
    UtlIniGroup *group_ptr = NULL;
    UtlIniValuePair *pair_ptr = NULL;

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
                        group_start = p + 1;
                    }
                    else if(*p == comment_char)
                    {
                        state = STAT_COMMENT;
                    }
                    else if(!isspace(*p))
                    {
                        state = STAT_KEY;
                        key_start = p;
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
                        utlStr_strTrim(group_start);
                        if ((group_ptr = utlMem_alloc(sizeof(UtlIniGroup), ALLOC_ZEROIZE)) != NULL) 
                        {
                            utl_ini_init_group_node(group_ptr);
                            group_ptr->group_name = utlMem_strdup(group_start);
                            utlLog_debug("[%s]", group_ptr->group_name);
                        }
                        else
                            goto error;
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
                        value_start = p + 1;
                    }
                    break;
                }
            case STAT_VALUE:
                {
                    if(*p == '\n' || *p == '\r')
                    {
                        *p = '\0';
                        state = STAT_NONE;
                        utlStr_strTrim(key_start);
                        utlStr_strTrim(value_start);
                        if (group_ptr != NULL) 
                        {
                            if (utl_ini_check_value(key_start, value_start)) 
                            {
                                if ((pair_ptr = utlMem_alloc(sizeof(UtlIniValuePair), ALLOC_ZEROIZE)) != NULL)
                                {
                                    pair_ptr->key = utlMem_strdup(key_start);
                                    pair_ptr->value = utlMem_strdup(value_start);
                                    dlist_add(&pair_ptr->pair_node, &group_ptr->pair_head);
                                    group_ptr->pair_num++;
                                    utlLog_debug("%s%c%s", pair_ptr->key, delim_char, pair_ptr->value);
                                }
                                else
                                    goto error;
                            }
                            else
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
        utlStr_strTrim(key_start);
        utlStr_strTrim(value_start);
        if (group_ptr != NULL) 
        {
            if (utl_ini_check_value(key_start, value_start)) 
            {
                if ((pair_ptr = utlMem_alloc(sizeof(UtlIniValuePair), ALLOC_ZEROIZE)) != NULL)
                {
                    pair_ptr->key = utlMem_strdup(key_start);
                    pair_ptr->value = utlMem_strdup(value_start);
                    dlist_add(&pair_ptr->pair_node, &group_ptr->pair_head);
                    group_ptr->pair_num++;
                    utlLog_debug("%s%c%s", pair_ptr->key, delim_char, pair_ptr->value);
                }
                else
                    goto error;
            }
            else
                goto error;
        }
        else
            goto error;
    }

    if(!func(&ini_group_head))
        goto error;

    utl_ini_cleanup(&ini_group_head);
    return TRUE;

error:
    utlLog_error("ini config file parse error, please check config file!");
    utl_ini_cleanup(&ini_group_head);
    return FALSE;
}
