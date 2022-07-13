/** @file AdvancedLoggerInternal.h

    Advanced Logger internal data structures


    Copyright (C) Microsoft Corporation. All rights reserved.
    SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef UEFI_INTERNAL_TYPE_H_
#define UEFI_INTERNAL_TYPE_H_

//
// Modifiers to abstract standard types to aid in debug of problems
//
#define CONST     const
#define STATIC    static
#define VOID      void

//
// Modifiers for Data Types used to self document code.
// This concept is borrowed for UEFI specification.
//
#ifndef IN
//
// Some other environments use this construct, so #ifndef to prevent
// multiple definition.
//
#define IN
#define OUT
#define OPTIONAL
#endif

#define EFIAPI

#define TRUE  ((BOOLEAN)(1==1))
#define FALSE  ((BOOLEAN)(0==1))

#define ALIGN_VALUE(Value, Alignment)  ((Value) + (((Alignment) - (Value)) & ((Alignment) - 1)))

//
// Use ANSI C 2000 stdint.h integer width declarations
//
#include <stdint.h>
typedef uint8_t   BOOLEAN;
typedef int8_t    INT8;
typedef uint8_t   UINT8;
typedef int16_t   INT16;
typedef uint16_t  UINT16;
typedef int32_t   INT32;
typedef uint32_t  UINT32;
typedef int64_t   INT64;
typedef uint64_t  UINT64;
typedef char      CHAR8;
typedef uint16_t  CHAR16;


///
/// EFI Time Abstraction:
///  Year:       1900 - 9999
///  Month:      1 - 12
///  Day:        1 - 31
///  Hour:       0 - 23
///  Minute:     0 - 59
///  Second:     0 - 59
///  Nanosecond: 0 - 999,999,999
///  TimeZone:   -1440 to 1440 or 2047
///
typedef struct {
  UINT16    Year;
  UINT8     Month;
  UINT8     Day;
  UINT8     Hour;
  UINT8     Minute;
  UINT8     Second;
  UINT8     Pad1;
  UINT32    Nanosecond;
  INT16     TimeZone;
  UINT8     Daylight;
  UINT8     Pad2;
} EFI_TIME;

#define MAX_UINT16  ((UINT16)0xFFFF)

typedef UINT64 UINTN;
typedef UINT64 EFI_PHYSICAL_ADDRESS;

#define SIGNATURE_16(A, B)  ((A) | (B << 8))
#define SIGNATURE_32(A, B, C, D)  (SIGNATURE_16 (A, B) | (SIGNATURE_16 (C, D) << 16))

#include <string.h>
#define CopyMem(a,b,c) memcpy(a,b,c)
#define ZeroMem(a,b)   memset(a,0,b);

#include <assert.h>
#define ASSERT(a)      assert(a)

#endif // UEFI_INTERNAL_TYPE_H_
