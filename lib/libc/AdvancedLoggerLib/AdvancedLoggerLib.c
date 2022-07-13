/** @file
  MM_CORE Arm implementation of Advanced Logger Library.

  Copyright (c) Microsoft Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
// TBD: This should not be here!!!
#include <pisoc_base_platform_def.h>

#include "../UefiBaseTypes.h"
#include "AdvancedLoggerInternal.h"

// #include <Library/AdvancedLoggerLib.h>
// #include <Library/AdvancedLoggerHdwPortLib.h>
// #include <Library/BaseMemoryLib.h>
// #include <Library/SynchronizationLib.h>

#include "AdvancedLoggerCommon.h"

#define ADV_LOGGER_MIN_SIZE  (65536)

STATIC ADVANCED_LOGGER_INFO  *mLoggerInfo = NULL;
STATIC UINT32              mBufferSize  = 0;
STATIC uint64_t              mMaxAddress  = 0;
STATIC BOOLEAN                  mInitialized = FALSE;

/**
  Validate Info Blocks

  The address of the ADVANCE_LOGGER_INFO block pointer is captured during the first DEBUG print.  The
  pointers LogBuffer and LogCurrent, and LogBufferSize, could be written to by untrusted code.  Here,
  we check that the pointers are within the allocated mLoggerInfo space, and that LogBufferSize, which
  is used in multiple places to see if a new message will fit into the log buffer, is valid.

  @param          NONE

  @return         BOOLEAN     TRUE = mLoggerInfo Block passes security checks
  @return         BOOLEAN     FALSE= mLoggerInfo Block failed security checks

**/
STATIC
BOOLEAN
ValidateInfoBlock (
  VOID
  )
{
  if (mLoggerInfo == NULL) {
    return FALSE;
  }

  if (mLoggerInfo->Signature != ADVANCED_LOGGER_SIGNATURE) {
    return FALSE;
  }

  if (mLoggerInfo->LogBuffer != (PA_FROM_PTR (mLoggerInfo + 1))) {
    return FALSE;
  }

  if ((mLoggerInfo->LogCurrent > mMaxAddress) ||
      (mLoggerInfo->LogCurrent < mLoggerInfo->LogBuffer))
  {
    return FALSE;
  }

  if (mBufferSize == 0) {
    mBufferSize = mLoggerInfo->LogBufferSize;
  } else if (mLoggerInfo->LogBufferSize != mBufferSize) {
    return FALSE;
  }

  return TRUE;
}

/**
  The logger Information Block is carved from the Trust Zone at a specific fixed address.

  This address is obtained from the ADV_LOGGER_BASE.  The size of the Advanced Logger
  buffer is obtained from PcdAdvancedLoggerPages.

  The following PCD settings are assumed:

  PcdAdvancedLoggerPeiInRAM    -- TRUE
  ADV_LOGGER_BASE        -- NOT NULL and pointer to memory to be used
  PcdAdvancedLoggerPages       -- > 64KB of pages
  PcdAdvancedLoggerCarBase     -- NOT USED, leave at default
  PcdAdvancedLoggerPreMemPages -- NOT USED, leave at default

  NOTE:  A DEBUG(()) statements here will cause recursion.  Insure that the recursion will be
         a straight path just to return the existing mLoggerInfo.

  @param       - None

  @returns     - NULL - No valid Advanced Logger Info block available
               - Pointer to a Valid Advanced Logger Info block.

 **/
ADVANCED_LOGGER_INFO *
AdvancedLoggerGetLoggerInfo (
  VOID
  )
{
  UINT32  BufferSize;

  if (!mInitialized) {
    mInitialized = TRUE;            // Only allow initialization once
    mLoggerInfo  = (ADVANCED_LOGGER_INFO *)(VOID *)(UINTN)ADV_LOGGER_BASE;
    ASSERT (mLoggerInfo != NULL);
    if ((mLoggerInfo == NULL) || (ADV_LOGGER_SIZE < ADV_LOGGER_MIN_SIZE)) {
      return NULL;
    }

    BufferSize = (UINT32)ADV_LOGGER_SIZE;
    ZeroMem ((VOID *)mLoggerInfo, BufferSize);
    mLoggerInfo->Signature     = ADVANCED_LOGGER_SIGNATURE;
    mLoggerInfo->Version       = ADVANCED_LOGGER_VERSION;
    mLoggerInfo->LogBuffer     = PA_FROM_PTR (mLoggerInfo + 1);
    mLoggerInfo->LogBufferSize = BufferSize - sizeof (ADVANCED_LOGGER_INFO);
    mLoggerInfo->LogCurrent    = mLoggerInfo->LogBuffer;
    // AdvancedLoggerHdwPortInitialize ();
    mLoggerInfo->HdwPortInitialized = TRUE;
    mLoggerInfo->InPermanentRAM     = TRUE;
    mMaxAddress                     = mLoggerInfo->LogBuffer + mLoggerInfo->LogBufferSize;
    mBufferSize                     = mLoggerInfo->LogBufferSize;
  }

  if (((mLoggerInfo) != NULL) && !ValidateInfoBlock ()) {
    mLoggerInfo = NULL;
    ERROR ("%s: LoggerInfo marked invalid\n", __FUNCTION__);
  }

  return mLoggerInfo;
}

uint64_t
AdvancedLoggerHdwPortWrite (
  IN uint64_t  DebugLevel,
  IN UINT8  *Buffer,
  IN uint64_t  NumberOfBytes
  )
{
  uint64_t  NumberReturned;

  NumberReturned = 0;
  while (NumberReturned < NumberOfBytes) {
    (VOID)putchar (Buffer[NumberReturned]);
    NumberReturned ++;
  }

  return NumberReturned;
}
