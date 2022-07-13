/*
 * Copyright (c) 2014-2021, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include <common/debug.h>

void
AdvancedLoggerWrite (
  uint64_t  ErrorLevel,
  char  *Buffer,
  uint64_t  NumberOfBytes
  );

#define get_num_va_args(_args, _lcount)				\
	(((_lcount) > 1)  ? va_arg(_args, long long int) :	\
	(((_lcount) == 1) ? va_arg(_args, long int) :		\
			    va_arg(_args, int)))

#define get_unum_va_args(_args, _lcount)				\
	(((_lcount) > 1)  ? va_arg(_args, unsigned long long int) :	\
	(((_lcount) == 1) ? va_arg(_args, unsigned long int) :		\
			    va_arg(_args, unsigned int)))

/*******************************************************************
 * Reduced format print for Trusted firmware.
 * The following type specifiers are supported by this print
 * %x - hexadecimal format
 * %s - string format
 * %d or %i - signed decimal format
 * %u - unsigned decimal format
 * %p - pointer format
 *
 * The following length specifiers are supported by this print
 * %l - long int (64-bit on AArch64)
 * %ll - long long int (64-bit on AArch64)
 * %z - size_t sized integer formats (64 bit on AArch64)
 *
 * The following padding specifiers are supported by this print
 * %0NN - Left-pad the number with 0s (NN is a decimal number)
 *
 * The print exits on all other formats specifiers other than valid
 * combinations of the above specifiers.
 *******************************************************************/
int vprintf(const char *fmt, va_list args)
{
  char adv_print_max[512];
  // int l_count;
  int count;

  count = vsnprintf (adv_print_max, sizeof (adv_print_max), fmt, args);

  AdvancedLoggerWrite(0xFFFFFFFF, adv_print_max, count);

  return count;
}

int printf(const char *fmt, ...)
{
	int count;
	va_list va;

	va_start(va, fmt);
	count = vprintf(fmt, va);
	va_end(va);

	return count;
}
