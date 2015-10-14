#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libsol-util/utl_logging.h>
#include <libsol-util/utl_strconv.h>
#include <libsol-util/utl_memory.h>
#include <libsol-util/utl_timer.h>
#include <libsol-util/utl_ini_parser.h>
#include "test.h"

void *tmrHandle = NULL;

void utl_logging_test(void)
{
    utlLog_init();

    /* test loglevel */
    utlLog_error("debug level = %d",(int)utlLog_getLevel());
    utlLog_error("log error test!");
    utlLog_notice("log notice test!");
    utlLog_debug("log debug test!");

    utlLog_setLevel(LOG_LEVEL_NOTICE);
    utlLog_error("debug level = %d",(int)utlLog_getLevel());
    utlLog_error("log error test!");
    utlLog_notice("log notice test!");
    utlLog_debug("log debug test!");

    utlLog_setLevel(LOG_LEVEL_DEBUG);
    utlLog_error("debug level = %d",(int)utlLog_getLevel());
    utlLog_error("log error test!");
    utlLog_notice("log notice test!");
    utlLog_debug("log debug test!");

    utlLog_error("********************************\n");

    /* test head mask */
    utlLog_error("head mask = %d", utlLog_getHeaderMask());
    utlLog_error("log head mask test!");

    utlLog_setHeaderMask(0x0001);
    utlLog_error("head mask = %d", utlLog_getHeaderMask());
    utlLog_error("log head mask test!");

    utlLog_setHeaderMask(0x0002);
    utlLog_error("head mask = %d", utlLog_getHeaderMask());
    utlLog_error("log head mask test!");

    utlLog_setHeaderMask(0x0004);
    utlLog_error("head mask = %d", utlLog_getHeaderMask());
    utlLog_error("log head mask test!");

    utlLog_setHeaderMask(0x0008);
    utlLog_error("head mask = %d", utlLog_getHeaderMask());
    utlLog_error("log head mask test!");

    utlLog_setHeaderMask(0x0003);
    utlLog_error("head mask = %d", utlLog_getHeaderMask());
    utlLog_error("log head mask test!");

    utlLog_setHeaderMask(0x0007);
    utlLog_error("head mask = %d", utlLog_getHeaderMask());
    utlLog_error("log head mask test!");

    utlLog_setHeaderMask(0x000f);
    utlLog_error("head mask = %d", utlLog_getHeaderMask());
    utlLog_error("log head mask test!");

    utlLog_error("********************************\n");

    /* log destination test */
    utlLog_error("log destination = %d", utlLog_getDestination());

    utlLog_setDestination(4);

    utlLog_error("log destination = %d", utlLog_getDestination());

    utlLog_setLogFile("/home/hyman/work/process/libutil/test/test.log");

    utlLog_error("log file = %s", utlLog_getLogFile());

    utlLog_cleanup();
}

void utl_memory_test(void)
{
    char *ptr1 = NULL;
    char *ptr2 = NULL;
    char *ptr3 = NULL;
    char *ptr4 = NULL;
    char *str  = "I'm sphantix, who are you?";

    utlLog_init();
    utlLog_setLevel(LOG_LEVEL_DEBUG);

    ptr1 = utlMem_alloc(256,ALLOC_ZEROIZE);
    strcpy(ptr1, "hello, world!");
    utlMem_dumpTraceAll();
    utlLog_debug("ptr1 = %s",ptr1);

    ptr2 = utlMem_strdup("I love the world");
    utlMem_dumpTraceAll();
    utlLog_debug("ptr2 = %s",ptr2);

    UTLMEM_REPLACE_STRING(ptr3, "I'm the king of the world");
    utlMem_dumpTraceAll();
    utlLog_debug("ptr3 = %s",ptr3);

    REPLACE_STRING_IF_NOT_EQUAL(ptr4, str);
    utlMem_dumpTraceAll();
    utlLog_debug("ptr4 = %s",ptr4);

    utlMem_free(ptr1);
    UTLMEM_FREE_BUF_AND_NULL_PTR(ptr2);
    UTLMEM_FREE_BUF_AND_NULL_PTR(ptr3);
    UTLMEM_FREE_BUF_AND_NULL_PTR(ptr4);

    utlLog_cleanup();
    return; 
}

void utl_timer_func1(void *handle)
{
    printf("this is the first timer func handler!\n");
}

void utl_timer_func2(void *handle)
{
    printf("this is the second timer func handler!\n");
    printf("str = %s\n",(char *)handle);
}

void utl_sleep(int seconds)
{
    sleep(seconds+1);
}

void utl_timer_test(void)
{
    char *string1 = "timer test!";
    UINT32 ms = 0;

    utlLog_init();
    utlLog_setLevel(LOG_LEVEL_DEBUG);

    utlTmr_init(&tmrHandle);

    utlTmr_set(tmrHandle, utl_timer_func1, NULL, 10*1000, "timer_test1");
    utlTmr_dumpEvents(tmrHandle);
    utlLog_debug("total timer events = %d", utlTmr_getNumberOfEvents(tmrHandle));

    utlTmr_set(tmrHandle, utl_timer_func2, string1 , 50*1000, "timer_test2");

    utlTmr_dumpEvents(tmrHandle);
    utlLog_debug("total timer events = %d", utlTmr_getNumberOfEvents(tmrHandle));

    if(utlTmr_getTimeToNextEvent(tmrHandle, &ms) == UTLRET_SUCCESS)
    {
        utl_sleep((ms/1000));
    }

    utlTmr_executeExpiredEvents(tmrHandle);
    utlTmr_dumpEvents(tmrHandle);
    utlLog_debug("total timer events = %d", utlTmr_getNumberOfEvents(tmrHandle));

    utlTmr_replaceIfSooner(tmrHandle, utl_timer_func2, string1 , 20*1000, "timer_test2");
    utlTmr_dumpEvents(tmrHandle);
    utlLog_debug("total timer events = %d", utlTmr_getNumberOfEvents(tmrHandle));

#if 1
    if(utlTmr_getTimeToNextEvent(tmrHandle, &ms) == UTLRET_SUCCESS)
    {
        utl_sleep((ms/1000));
    }

    utlTmr_executeExpiredEvents(tmrHandle);
    utlTmr_dumpEvents(tmrHandle);
    utlLog_debug("total timer events = %d", utlTmr_getNumberOfEvents(tmrHandle));
#endif

    utlTmr_cleanup(&tmrHandle);
    utlLog_cleanup();
}

UBOOL8 fPortSet(const char *name, const char *value)
{
    utlLog_debug("name = %s, value = %s", name, value);
    return TRUE;
}

UBOOL8 fNumSet(const char *name, const char *value)
{
    utlLog_debug("name = %s, value = %s", name, value);
    return TRUE;
}

UBOOL8 fUrlSet(const char *name, const char *value)
{
    utlLog_debug("name = %s, value = %s", name, value);
    return TRUE;
}

UBOOL8 fPathSet(const char *name, const char *value)
{
    utlLog_debug("name = %s, value = %s", name, value);
    return TRUE;
}

IniNodeDesc basicDesc[] = {
    {"port", fPortSet, NULL},
    {"num", fNumSet, NULL},
    {NULL, NULL}
};

IniNodeDesc serverDesc[] = {
    {"url", fUrlSet, NULL},
    {"path", fPathSet, NULL},
    {NULL, NULL}
};

IniNodeDesc groupDesc[] = {
    {"Basic", NULL, basicDesc},
    {"Server", NULL, serverDesc},
    {NULL, NULL}
};

UBOOL8 pairHandler(struct __IniNodeDesc *leafDesc, DlistNode *head)
{
    int i;

    UtlIniValuePair *pair_ptr = NULL;

    dlist_for_each_entry(pair_ptr, head, pair_node)
    {
        for(i=0; leafDesc[i].tagName != NULL; i++)
        {
            if(!utlStr_strcmp(pair_ptr->key, leafDesc[i].tagName))
            {
                if(!leafDesc[i].setIniFunc(pair_ptr->key, pair_ptr->value))
                    return FALSE;
            }
        }
    }

    return TRUE;
}

UBOOL8 groupHandler(DlistNode *head)
{
    int i;

    UtlIniGroup *group_ptr = NULL;

    dlist_for_each_entry(group_ptr, head, group_node)
    {
        for(i=0; groupDesc[i].tagName != NULL; i++)
        {
            if(!utlStr_strcmp(group_ptr->group_name, groupDesc[i].tagName))
            {
                if(!pairHandler(groupDesc[i].leafNode, &group_ptr->pair_head))
                    return FALSE;
            }
        }
    }

    return TRUE;
}

void utl_ini_parser_test(void)
{
    char* buffer = strdup("[Basic]\nport=10\nnum=20\n[Server]\nurl=http://baidu.com\npath=/auth100");

    utlLog_init();
    utlLog_setLevel(LOG_LEVEL_DEBUG);

    utl_ini_parser(buffer,'#','=',groupHandler);

    utlLog_cleanup();
}

void print_usage(void)
{
    printf("Usage: test [syslog|memory|timer|iniparser]\n");
    printf("Example:\n");
    printf("       test syslog       --log utilities test\n");
}

int main(int argc, const char *argv[])
{
    if(argc != 2)
    {
        print_usage();
    }
    else
    {
        if(!strncasecmp(argv[1], "syslog", 6))
        {
            utl_logging_test();
        }
        else if (!strncasecmp(argv[1], "memory", 6)) 
        {
            utl_memory_test();
        }
        else if (!strncasecmp(argv[1], "timer", 5)) 
        {
            utl_timer_test();
        }
        else if (!strncasecmp(argv[1], "iniparser", 9)) 
        {
            utl_ini_parser_test();
        }
        else
        {
            print_usage();
        }
    }
    return 0;
}
