#include <stddef.h>
#include <stdint.h>

#include "../UefiBaseTypes.h"
#include "../SynchronizationLib.h"
#include "../TimerLib.h"
#include "AdvancedLoggerInternal.h"
#include "AdvancedLoggerCommon.h"

/**
  Write data from buffer into the in memory logging buffer.

  Writes NumberOfBytes data bytes from Buffer to the logging buffer.

  @param  DebugLevel       Debug level of the message
  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to be written to the Advanced Logger log.

  @retval LoggerInfo       Returns the logger info block. Returns NULL if it cannot
                           be located. This occurs prior to SEC completion.
**/
static
ADVANCED_LOGGER_INFO *
AdvancedLoggerMemoryLoggerWrite (
        uint64_t  DebugLevel,
  CONST CHAR8  *Buffer,
        uint64_t  NumberOfBytes
  )
{
  ADVANCED_LOGGER_INFO           *LoggerInfo;
  uint64_t           CurrentBuffer;
  uint64_t           NewBuffer;
  uint64_t           OldValue;
  uint32_t                         OldSize;
  uint32_t                         NewSize;
  uint32_t                         CurrentSize;
  uint64_t                          EntrySize;
  uint64_t                          UsedSize;
  ADVANCED_LOGGER_MESSAGE_ENTRY  *Entry;

  if ((NumberOfBytes == 0) || (Buffer == NULL)) {
    return NULL;
  }

  if (NumberOfBytes > MAX_UINT16) {
    return NULL;
  }

  LoggerInfo = AdvancedLoggerGetLoggerInfo ();
  if (LoggerInfo != NULL) {
    EntrySize = MESSAGE_ENTRY_SIZE (NumberOfBytes);
    do {
      CurrentBuffer = LoggerInfo->LogCurrent;
      UsedSize      = (uint64_t)(CurrentBuffer - LoggerInfo->LogBuffer);
      if ((UsedSize >= LoggerInfo->LogBufferSize) ||
          ((LoggerInfo->LogBufferSize - UsedSize) < EntrySize))
      {
        //
        // Update the number of bytes of log that have not been captured
        //
        do {
          CurrentSize = LoggerInfo->DiscardedSize;
          NewSize     = CurrentSize + (uint32_t)NumberOfBytes;
          OldSize     = InterlockedCompareExchange32 (
                          (uint32_t *)&LoggerInfo->DiscardedSize,
                          (uint32_t)CurrentSize,
                          (uint32_t)NewSize
                          );
        } while (OldSize != CurrentSize);

        return LoggerInfo;
      }

      NewBuffer = PA_FROM_PTR ((CHAR8_FROM_PA (CurrentBuffer) + EntrySize));
      OldValue  = InterlockedCompareExchange64 (
                    (UINT64 *)&LoggerInfo->LogCurrent,
                    (UINT64)CurrentBuffer,
                    (UINT64)NewBuffer
                    );
    } while (OldValue != CurrentBuffer);

    Entry            = (ADVANCED_LOGGER_MESSAGE_ENTRY *)PTR_FROM_PA (CurrentBuffer);
    Entry->TimeStamp = GetPerformanceCounter ();    // AdvancedLoggerGetTimeStamp();

    // DebugLevel is defined as a uint64_t, so it is 32 bits in PEI and 64 bits in DXE.
    // However, the DEBUG_* values and the PcdFixedDebugPrintErrorLevel are only 32 bits.
    Entry->DebugLevel = (uint32_t)DebugLevel;
    Entry->MessageLen = (UINT16)NumberOfBytes;
    CopyMem (Entry->MessageText, Buffer, NumberOfBytes);
    Entry->Signature = MESSAGE_ENTRY_SIGNATURE;
  }

  return LoggerInfo;
}

/**
  Write data from buffer to possible debugging devices.

  This is the interface from PeiCore
  This is also called by the Ppi

  Writes NumberOfBytes data bytes from Buffer to the debugging devices.

  @param  DebugLevel       Error level of items top be printed
  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to be written to the Advanced Logger log.

  @retval LoggerInfo       Returns the logger info block. Returns NULL if it cannot
                           be located. This occurs prior to SEC completion.

**/
void
AdvancedLoggerWrite (
        uint64_t  DebugLevel,
  CONST CHAR8  *Buffer,
        uint64_t  NumberOfBytes
  )
{
  ADVANCED_LOGGER_INFO  *LoggerInfo;

  // All messages go to the in memory log.
  LoggerInfo = AdvancedLoggerMemoryLoggerWrite (DebugLevel, Buffer, NumberOfBytes);

  // Only selected messages go to the hdw port.
  if ((LoggerInfo != NULL) && (!LoggerInfo->HdwPortDisabled))
  {
    AdvancedLoggerHdwPortWrite (DebugLevel, (UINT8 *)Buffer, NumberOfBytes);
  }
}
