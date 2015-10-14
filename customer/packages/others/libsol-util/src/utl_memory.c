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

#include <string.h>
#ifdef DESKTOP_LINUX
#include <execinfo.h>  /* for backtrace */
#endif
#include "utl_list.h"
#include "utl_memory.h"
#include "utl_time.h"
#include "utl_logging.h"
#include "utl_assert.h"
#include "bget.h"

extern int backtrace(void **buffer, int size);


/** Macro to round up to nearest 4 byte length */
#define ROUNDUP4(s)  (((s) + 3) & 0xfffffffc)

/** Macro to calculate how much we need to allocate for a given user size request.
 *
 * We need the header even when not doing MEM_DEBUG to keep
 * track of the size and allocFlags that was passed in during cmsMem_alloc.
 * This info is needed during cmsMem_realloc.
 */
#ifdef UTL_MEM_DEBUG
#define REAL_ALLOC_SIZE(s) (UTL_MEM_HEADER_LENGTH + ROUNDUP4(s) + UTL_MEM_FOOTER_LENGTH)
#else
#define REAL_ALLOC_SIZE(s) (UTL_MEM_HEADER_LENGTH + (s))
#endif

static UtlMemStats mStats;

#ifdef UTL_MEM_LEAK_TRACING

#define NUM_STACK_ENTRIES   15

typedef struct alloc_record {
   DlistNode dlist;
   unsigned char *bufAddr;
   UINT32 userSize;
   UBOOL8 isClone;
   UBOOL8 dumpThisClone;
   UINT32 seq;
   void *stackAddr[NUM_STACK_ENTRIES];
} AllocRecord;

static UINT32 allocSeq=0;
DLIST_HEAD(glbAllocRec);

static void initAllocSeq(void)
{
   UtlTimestamp tms;

   if (allocSeq > 0)
   {
      return;
   }

   utlTm_get(&tms);

   srand(tms.nsec);
   allocSeq = rand() << 16;
   utlLog_debug("allocSeq=%lu", allocSeq);

   return;
}

static void garbageCollectAllocRec(void);
#endif /* UTL_MEM_LEAK_TRACING */

void *utlMem_alloc(UINT32 size, UINT32 allocFlags)
{
    void *buf;
    UINT32 allocSize;

#ifdef UTL_MEM_LEAK_TRACING
    initAllocSeq();
#endif

    allocSize = REAL_ALLOC_SIZE(size);

    buf = osl_malloc(allocSize);
    if (buf)
    {
        mStats.bytesAllocd += size;
        mStats.numAllocs++;
    }


    if (buf != NULL)
    {
        UINT32 *intBuf = (UINT32 *) buf;
        UINT32 intSize = allocSize / sizeof(UINT32);


        if (allocFlags & ALLOC_ZEROIZE)
        {
            memset(buf, 0, allocSize);
        }
#ifdef UTL_MEM_POISON_ALLOC_FREE
        else
        {
            /*
             * Set alloc'ed buffer to garbage to catch use-before-init.
             * But we also allocate huge buffers for storing image downloads.
             * Don't bother writing garbage to those huge buffers.
             */
            if (allocSize < 64 * 1024)
            {
                memset(buf, UTL_MEM_ALLOC_PATTERN, allocSize);
            }
        }
#endif

        /*
         * Record the allocFlags in the first word, and the 
         * size of user buffer in the next 2 words of the buffer.
         * Make 2 copies of the size in case one of the copies gets corrupted by
         * an underflow.  Make one copy the XOR of the other so that there are
         * not so many 0's in size fields.
         */
        intBuf[0] = allocFlags;
        intBuf[1] = size;
        intBuf[2] = intBuf[1] ^ 0xffffffff;

        buf = &(intBuf[3]); /* this gets returned to user */

#ifdef UTL_MEM_DEBUG
        {
            UINT8 *charBuf = (UINT8 *) buf;
            UINT32 i, roundup4Size = ROUNDUP4(size);

            for (i=size; i < roundup4Size; i++)
            {
                charBuf[i] = UTL_MEM_FOOTER_PATTERN & 0xff;
            }

            intBuf[intSize - 1] = UTL_MEM_FOOTER_PATTERN;
            intBuf[intSize - 2] = UTL_MEM_FOOTER_PATTERN;
        }
#endif

#ifdef UTL_MEM_LEAK_TRACING
        {
            AllocRecord *allocRec;
            if (!(allocRec = calloc(1, sizeof(AllocRecord))))
            {
                utlLog_error("could not malloc a record to track alloc");
            }
            else
            {
                allocRec->bufAddr = buf;
                allocRec->userSize = size;
                allocRec->seq = allocSeq++;
                backtrace(allocRec->stackAddr, NUM_STACK_ENTRIES);
                /*
                 * new allocs are placed at the beginning of the list, right after
                 * the head.
                 */
                dlist_append((struct dlist_node *)allocRec, &glbAllocRec);
            }

            /*
             * do periodic garbage collection on the allocRecs which point
             * to shmBuf's that has been freed by another app.
             */
            if ((allocSeq % 2000) == 0)
            {
                utlLog_debug("Starting allocRec garbage collection");
                garbageCollectAllocRec();
                utlLog_debug("garbage collection done");
            }
        }
#endif
    }
    return buf;
}

void *utlMem_realloc(void *origBuf, UINT32 size)
{
    void *buf;
    UINT32 origSize, origAllocSize, origAllocFlags;
    UINT32 allocSize;
    UINT32 *intBuf;

    if (origBuf == NULL)
    {
        utlLog_error("cannot take a NULL buffer");
        return NULL;
    }

    if (size == 0)
    {
        utlMem_free(origBuf);
        return NULL;
    }

    allocSize = REAL_ALLOC_SIZE(size);

    intBuf = (UINT32 *) (((UINT32) origBuf) - UTL_MEM_HEADER_LENGTH);

    origAllocFlags = intBuf[0];
    origSize = intBuf[1];

    /* sanity check the original length */
    if (intBuf[1] != (intBuf[2] ^ 0xffffffff))
    {
        utlLog_error("memory underflow detected, %d %d", intBuf[1], intBuf[2]);
        utlAst_assert(0);
        return NULL;
    }

    origAllocSize = REAL_ALLOC_SIZE(origSize);

    if (allocSize <= origAllocSize)
    {
        /* currently, I don't shrink buffers, but could in the future. */
        return origBuf;
    }

    buf = utlMem_alloc(allocSize, origAllocFlags);
    if (buf != NULL)
    {
        /* got new buffer, copy orig buffer to new buffer */
        memcpy(buf, origBuf, origSize);
        utlMem_free(origBuf);
    }
    else
    {
        /*
         * We could not allocate a bigger buffer.
         * Return NULL but leave the original buffer untouched.
         */
    }

    return buf;
}

void utlMem_free(void *buf)
{
    UINT32 size;

    if (buf != NULL)
    {
        UINT32 *intBuf = (UINT32 *) (((UINT32) buf) - UTL_MEM_HEADER_LENGTH);

#ifdef UTL_MEM_LEAK_TRACING
        {
            AllocRecord *allocRec;
            dlist_for_each_entry(allocRec, &glbAllocRec, dlist)
                if (allocRec->bufAddr == buf)
                    break;

            if ((DlistNode *) allocRec != &glbAllocRec)
            {
                dlist_del((struct dlist_node *) allocRec);
                free(allocRec);
            }
        }
#endif

        size = intBuf[1];

        if (intBuf[1] != (intBuf[2] ^ 0xffffffff))
        {
            utlLog_error("memory underflow detected, %d %d", intBuf[1], intBuf[2]);
            utlAst_assert(0);
            return;
        }

#ifdef UTL_MEM_DEBUG
        {
            UINT32 allocSize, intSize, roundup4Size, i;
            UINT8 *charBuf = (UINT8 *) buf;

            allocSize = REAL_ALLOC_SIZE(intBuf[1]);
            intSize = allocSize / sizeof(UINT32);
            roundup4Size = ROUNDUP4(intBuf[1]);

            for (i=intBuf[1]; i < roundup4Size; i++)
            {
                if (charBuf[i] != (UINT8) (UTL_MEM_FOOTER_PATTERN & 0xff))
                {
                    utlLog_error("memory overflow detected at idx=%d 0x%x 0x%x 0x%x",
                            i, charBuf[i], intBuf[intSize-1], intBuf[intSize-2]);
                    utlAst_assert(0);
                    return;
                }
            }

            if ((intBuf[intSize - 1] != UTL_MEM_FOOTER_PATTERN) ||
                    (intBuf[intSize - 2] != UTL_MEM_FOOTER_PATTERN))
            {
                utlLog_error("memory overflow detected, 0x%x 0x%x",
                        intBuf[intSize - 1], intBuf[intSize - 2]);
                utlAst_assert(0);
                return;
            }

#ifdef UTL_MEM_POISON_ALLOC_FREE
            /*
             * write garbage into buffer which is about to be freed to detect
             * users of freed buffers.
             */
            memset(intBuf, UTL_MEM_FREE_PATTERN, allocSize);
#endif
        }

#endif  /* UTL_MEM_DEBUG */

        buf = intBuf;  /* buf points to real start of buffer */

        osl_free(buf);
        mStats.bytesAllocd -= size;
        mStats.numFrees++;
    }
}

char *utlMem_strdup(const char *str)
{
   return utlMem_strdupFlags(str, 0);
}

char *utlMem_strdupFlags(const char *str, UINT32 flags)
{
   UINT32 len;
   void *buf;

   if (str == NULL)
   {
      return NULL;
   }

   /* this is somewhat dangerous because it depends on str being NULL
    * terminated.  Use strndup/strlen if not sure the length of the string.
    */
   len = strlen(str);

   buf = utlMem_alloc(len+1, flags);
   if (buf == NULL)
   {
      return NULL;
   }

   strncpy((char *) buf, str, len+1);

   return ((char *) buf);
}

char *utlMem_strndup(const char *str, UINT32 maxlen)
{
   return utlMem_strndupFlags(str, maxlen, 0);
}

char *utlMem_strndupFlags(const char *str, UINT32 maxlen, UINT32 flags)
{
   UINT32 len;
   char *buf;

   if (str == NULL)
   {
      return NULL;
   }

   len = utlMem_strnlen(str, maxlen, NULL);

   buf = (char *) utlMem_alloc(len+1, flags);
   if (buf == NULL)
   {
      return NULL;
   }

   strncpy(buf, str, len);
   buf[len] = 0;

   return buf;
}

UINT32 utlMem_strnlen(const char *str, UINT32 maxlen, UBOOL8 *isTerminated)
{
   UINT32 len=0;

   while ((len < maxlen) && (str[len] != 0))
   {
      len++;
   }

   if (isTerminated != NULL)
   {
      *isTerminated = (str[len] == 0);
   }

   return len;
}

void utlMem_getStats(UtlMemStats *stats)
{
   /* the private heap memory stats can come directly from our data structure */
   stats->bytesAllocd = mStats.bytesAllocd;
   stats->numAllocs = mStats.numAllocs;
   stats->numFrees = mStats.numFrees;

   return;
}

#define KB_IN_B  1024

void utlMem_dumpMemStats()
{
   UtlMemStats memStats;

   utlMem_getStats(&memStats);

   printf("Heap bytes in-use     : %06d\n", memStats.bytesAllocd);
   printf("Heap allocs           : %06d\n", memStats.numAllocs);
   printf("Heap frees            : %06d\n", memStats.numFrees);
   printf("Heap alloc/free delta : %06d\n", memStats.numAllocs - memStats.numFrees);

   return;
}

#ifdef UTL_MEM_LEAK_TRACING

static const int FSHIFT = 16;              /* nr of bits of precision */
#define FIXED_1         (1<<FSHIFT)     /* 1.0 as fixed-point */
#define LOAD_INT(x) ((x) >> FSHIFT)
#define LOAD_FRAC(x) LOAD_INT(((x) & (FIXED_1-1)) * 100)

#define abs(s) ((s) < 0? -(s) : (s))

#ifndef DESKTOP_LINUX
int backtrace(void **buffer, int size)
{
    unsigned long *addr;
    unsigned long *ra;
    unsigned long *sp;
    size_t ra_offset;
    size_t stack_size;
    int depth;

    if (!size)
        return 0;
    if (!buffer || size < 0)
        return -1;

    /* Get current $ra and $sp */
    __asm__ __volatile__ (
            "	move %0, $ra\n"
            "	move %1, $sp\n"
            : "=r"(ra), "=r"(sp)
            );

    /* Scanning to find the size of the current stack-frame */
    stack_size = 0;
    for (addr = (unsigned long *)backtrace; !stack_size; ++addr) {
        if ((*addr & 0xffff0000) == 0x27bd0000) /* addiu sp, sp, sz */
            stack_size = abs((short)(*addr & 0xffff));
        else if (*addr == 0x03e00008) /* jr ra */
            break;
    }
    sp = (unsigned long *)((unsigned long)sp + stack_size);

    /* Repeat backward scanning */
    for (depth = 0; depth < size && ra; ++depth) {
        buffer[depth] = ra;
        ra_offset = 0;
        stack_size = 0;

        for (addr = ra; !ra_offset || !stack_size; --addr) {
            switch (*addr & 0xffff0000) {
                case 0x27bd0000: /* addiu sp, sp, -stack_size */
                    stack_size = abs((short)(*addr & 0xffff));
                    break;
                case 0xafbf0000: /* sw ra, offset */
                    ra_offset = (short)(*addr & 0xffff);
                    break;
                case 0x3c1c0000: /* lui gp, constant */
                    return depth + 1;
                default:
                    break;
            }
        }
        ra = *(unsigned long **)((unsigned long)sp + ra_offset);
        sp = (unsigned long *)((unsigned long)sp + stack_size);
    }
    return depth;
}
#endif

static void print_backtrace(void **array, int size)
{
    FILE *fp;
    int i;

    fp = fopen("/proc/self/maps","r");

    for (i = 0; i < size && 0 < (UINT32) array[i]; i++) {
        char line[1024];
        int found = 0;

        rewind(fp);
        while (fgets(line, sizeof(line), fp)) {
            char lib[1024];
            void *start, *end;
            unsigned int offset;
            int n = sscanf(line, "%p-%p %*s %x %*s %*d %s",
                    &start, &end, &offset, lib);
            if (n == 4 && array[i] >= start && array[i] < end) {
#ifdef DESKTOP_LINUX
                offset = array[i] - start;
#else
                if (array[i] < (void*)0x10000000)
                    offset = (unsigned int)array[i];
                else
                    offset += array[i] - start;
#endif

                printf("#%d  [%08x] in %s\n", i, offset, lib);
                found = 1;
                break;
            }
        }
        if (!found)
            printf("#%d  [%08x]\n", i, (unsigned)array[i]);
    }
    fclose(fp);
}


static void dumpAllocRec(AllocRecord *allocRec, UBOOL8 dumpStackTrace)
{
    printf("allocRec size=%d buf=%p seq=%u\n", allocRec->userSize, allocRec->bufAddr, allocRec->seq);
    if (dumpStackTrace)
    {
        print_backtrace(allocRec->stackAddr, NUM_STACK_ENTRIES);
    }

    return;
}


static UBOOL8 compareStacks(void **stack1, void **stack2)
{
    UBOOL8 same=TRUE;
    UINT32 i;

    for (i=0; i < NUM_STACK_ENTRIES && same; i++)
    {
        if (stack1[i] != stack2[i])
        {
            same = FALSE;
        }
    }

    return same;
}


static void dumpNumTraces(UINT32 max)
{
    AllocRecord *allocRec;
    UINT32 count=0;

    dlist_for_each_entry(allocRec, &glbAllocRec, dlist)
    {
        count++;
        if (max > 0 && count > max)
        {
            break;
        }

        dumpAllocRec(allocRec, TRUE);
    }
}

void utlMem_dumpTraceAll()
{
   dumpNumTraces(0);
}

void utlMem_dumpTrace50()
{
   dumpNumTraces(50);
}

#define CLONE_COUNT_THRESH  5

void utlMem_dumpTraceClones()
{
    AllocRecord *allocRec;
    AllocRecord *allocRec2;
    UINT32 cloneCount;
    UBOOL8 anyClonesFound=FALSE;

    /* first clear all the isClone flags in the allocRecs */
    dlist_for_each_entry(allocRec, &glbAllocRec, dlist)
    {
        allocRec->isClone = FALSE;
        allocRec->dumpThisClone = FALSE;
    }

    dlist_for_each_entry(allocRec, &glbAllocRec, dlist)
    {
        cloneCount = 1;  /* the current one counts as a clone too */

        /* clear all previous dumpThisClone flags from previous clone search */
        allocRec2 = (AllocRecord *) allocRec->dlist.next;
        while (((DlistNode *) allocRec2) != &glbAllocRec)
        {
            allocRec2->dumpThisClone = FALSE;
            allocRec2 = (AllocRecord *) allocRec2->dlist.next;
        }

        /* search for clones of allocRec */
        allocRec2 = (AllocRecord *) allocRec->dlist.next;
        while (((DlistNode *) allocRec2) != &glbAllocRec)
        {
            if ((!allocRec2->isClone) &&
                    (allocRec->userSize == allocRec2->userSize) &&
                    (compareStacks(allocRec->stackAddr, allocRec2->stackAddr)))
            {
                /* found a clone */
                allocRec2->isClone = TRUE;
                allocRec2->dumpThisClone = TRUE;
                cloneCount++;
            }
            allocRec2 = (AllocRecord *) allocRec2->dlist.next;
        }

        if (cloneCount >= CLONE_COUNT_THRESH)
        {
            printf("==== Found %d clones ====\n", cloneCount);
            anyClonesFound = TRUE;
            dumpAllocRec(allocRec, TRUE);

            allocRec2 = (AllocRecord *) allocRec->dlist.next;
            while (((DlistNode *) allocRec2) != &glbAllocRec)
            {
                if (allocRec2->dumpThisClone)
                {
                    dumpAllocRec(allocRec2, FALSE);
                }
                allocRec2 = (AllocRecord *) allocRec2->dlist.next;
            }
            printf("\n\n");
        }
    }

    if (!anyClonesFound)
    {
        printf("==== No cloned records found ====\n");
    }

    return;
}


static void garbageCollectAllocRec()
{
    return;
}
#endif  /* UTL_MEM_LEAK_TRACING */
