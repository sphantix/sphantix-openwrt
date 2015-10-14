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

#ifndef __UTL_MEMORY_H__
#define __UTL_MEMORY_H__

#include "utl.h"
#include "./os/osl_memory.h"

/** zeroize the buffer before returning it to the caller. */
#define ALLOC_ZEROIZE          0x01

#ifdef __cplusplus
extern "C" {
#endif

void *utlMem_alloc(UINT32 size, UINT32 allocFlags);
void *utlMem_realloc(void *origBuf, UINT32 size);
void utlMem_free(void *buf);
char *utlMem_strdup(const char *str);
char *utlMem_strdupFlags(const char *str, UINT32 flags);
char *utlMem_strndup(const char *str, UINT32 maxlen);
char *utlMem_strndupFlags(const char *str, UINT32 maxlen, UINT32 flags);
UINT32 utlMem_strnlen(const char *str, UINT32 maxlen, UBOOL8 *isTerminated);

/** Free a buffer and set the pointer to null.
 */
#define UTLMEM_FREE_BUF_AND_NULL_PTR(p) \
   do { \
      if ((p) != NULL) {utlMem_free((p)); (p) = NULL;}   \
   } while (0)
   

/** Free the existing char pointer and set it to a copy of the specified string.
 */
#define UTLMEM_REPLACE_STRING(p, s) \
   do {\
      if ((p) != NULL) {utlMem_free((p));} \
      (p) = utlMem_strdup((s)); \
   } while (0)


/** Free the existing char pointer and set it to a copy of the specified string
 *  and specify the way the string is allocated.
 */
#define UTLMEM_REPLACE_STRING_FLAGS(p, s, f) \
   do {\
      if ((p) != NULL) {utlMem_free((p));} \
      (p) = utlMem_strdupFlags((s), (f)); \
   } while (0)


/** Replace the existing string p if it is different than the new string s
 */
#define REPLACE_STRING_IF_NOT_EQUAL(p, s)    \
	if ((p) != NULL) { \
      if (strcmp((p), (s))) { utlMem_free((p)); (p) = utlMem_strdup((s)); } \
	} else { \
		(p) = utlMem_strdup((s));                 \
	} 


/** Replace the existing string p if it is different than the new string s
 * and specify the way the string is allocated.
 */
#define REPLACE_STRING_IF_NOT_EQUAL_FLAGS(p, s, f)     \
	if ((p) != NULL) { \
      if (strcmp((p), (s))) {utlMem_free((p)); (p) = utlMem_strdupFlags((s), (f)); } \
	} else { \
		(p) = utlMem_strdupFlags((s), (f));       \
	}

/* Memory Statistics structure.
 */
typedef struct
{
   UINT32 bytesAllocd;   /**< Number of private heap bytes alloc'ed */
   UINT32 numAllocs;     /**< Number of private heap allocs */
   UINT32 numFrees;      /**< Number of private heap frees */
} UtlMemStats;

void utlMem_getStats(UtlMemStats *stats);
void utlMem_dumpMemStats();

#ifdef UTL_MEM_LEAK_TRACING
void utlMem_dumpTraceAll();
void utlMem_dumpTrace50();
void utlMem_dumpTraceClones();
#endif

#ifdef __cplusplus
} /* end of extern "C" */
#endif

/** How much debug information to put at the beginning of a malloc'd block,
 *  to keep track of original length and allocation flags (and also to
 *  detect buffer under-write).
 *
 * We allocate a header regardless of UTL_MEM_DEBUG.
 *
 * The memory code assumes this value is 12, so you'll need to modify
 * utl_memory.c if you change this constant.
 */
#define UTL_MEM_HEADER_LENGTH   12


/** How much debug information to put at the end of a malloc'd block,
 *  (to detect buffer over-write) used only if UTL_MEM_DEBUG is defined.
 *
 * The memory code assumes this value is 8, so you'll need to modify
 * utl_memory.c if you change this constant.
 */
#define UTL_MEM_FOOTER_LENGTH   8


/** Pattern put at the memory block footer (to detect buffer over-write).
 *
 * Used only if UTL_MEM_DEBUG is defined.
 * If allocation is not on 4 byte boundary, the bytes between the user
 * requested bytes and the 4 byte boundary are filled with the 
 * last byte of the UTL_MEM_FOOTER_PATTERN.
 */
#define UTL_MEM_FOOTER_PATTERN   0xfdfdfdfd


/** Pattern put in freshly allocated memory blocks, which do not have
 *  ALLOC_ZEROIZE flag set (to detect use before initialization.)
 *
 * Used only if UTL_MEM_DEBUG is defined.
 */
#define UTL_MEM_ALLOC_PATTERN    ((UINT8) 0x95)


/** Pattern put in freed memory blocks (to detect use after free.)
 *
 * Used only if UTL_MEM_DEBUG is defined.
 */
#define UTL_MEM_FREE_PATTERN     ((UINT8) 0x97)

#endif /* __UTL_MEMORY_H__ */
