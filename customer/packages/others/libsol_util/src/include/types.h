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

#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>  /* for the various integer types */

/*!\file types.h
 * \brief Various commonly used, but OS dependent definitions are defined here.
 *
 *  This file is for Linux.
 */

#ifndef NUMBER_TYPES_ALREADY_DEFINED
#define NUMBER_TYPES_ALREADY_DEFINED

typedef uint64_t   UINT64;
typedef int64_t    SINT64;
typedef uint32_t   UINT32;
typedef int32_t    SINT32;
typedef uint16_t   UINT16;
typedef int16_t    SINT16;
typedef uint8_t    UINT8;
typedef int8_t     SINT8;

#endif /* NUMBER_TYPES_ALREADY_DEFINED */

/**Boolean type; use 1 byte only, possible values are TRUE(1) or FALSE(0) only.
 *
 */
#ifndef BOOL_TYPE_ALREADY_DEFINED
#define BOOL_TYPE_ALREADY_DEFINED

typedef uint8_t    UBOOL8;

#endif /* BOOL_TYPE_ALREADY_DEFINED */


/* Base64 encoded string representation of binary data. */
typedef char *     BASE64;


/* Hex encoded string representation of binary data. */
typedef char *     HEXBINARY;


/* String representation of date and time. */
typedef char *     DATETIME;


/* Invalid file descriptor number */
#define INVALID_FD  (-1)


/* Invalid process id. */
#define INVALID_PID   0

/* TRUE = 1 */
#ifndef TRUE
#define TRUE  1
#endif

/* FALSE = 0 */
#ifndef FALSE
#define FALSE 0
#endif

/* Maximum value for a UINT64 */
#define MAX_UINT64 18446744073709551615ULL

/* Maximum value for a SINT64 */
#define MAX_SINT64 9223372036854775807LL

/* Minimum value for a SINT64 */
#define MIN_SINT64 (-1 * MAX_SINT64 - 1)

/* Maximum value for a UINT32 */
#define MAX_UINT32 4294967295U

/* Maximum value for a SINT32 */
#define MAX_SINT32 2147483647

/* Minimum value for a SINT32 */
#define MIN_SINT32 (-2147483648)

/* Maximum value for a UINT16 */
#define MAX_UINT16  65535

/* Maximum value for a SINT16 */
#define MAX_SINT16  32767

/* Minimum value for a SINT16 */
#define MIN_SINT16  (-32768)

/* This is common used string length types. */
#define BUFLEN_4        4     //!< buffer length 4
#define BUFLEN_8        8     //!< buffer length 8
#define BUFLEN_16       16    //!< buffer length 16
#define BUFLEN_18       18    //!< buffer length 18 -- for ppp session id
#define BUFLEN_24       24    //!< buffer length 24 -- mostly for password
#define BUFLEN_32       32    //!< buffer length 32
#define BUFLEN_40       40    //!< buffer length 40
#define BUFLEN_48       48    //!< buffer length 48
#define BUFLEN_64       64    //!< buffer length 64
#define BUFLEN_80       80    //!< buffer length 80
#define BUFLEN_128      128   //!< buffer length 128
#define BUFLEN_256      256   //!< buffer length 256
#define BUFLEN_264      264   //!< buffer length 264
#define BUFLEN_512      512   //!< buffer length 512
#define BUFLEN_1024     1024  //!< buffer length 1024

#define MAC_ADDR_LEN    6     //!< Mac address len in an array of 6 bytes
#define MAC_STR_LEN     17    //!< Mac String len with ":". eg: xx:xx:xx:xx:xx:x

#endif /* __TYPES_H__ */
