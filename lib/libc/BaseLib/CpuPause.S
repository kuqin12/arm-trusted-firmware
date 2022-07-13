//  Implementation of synchronization functions for ARM architecture (AArch64)
//
//  Copyright (c) 2012-2015, ARM Limited. All rights reserved.
//  Copyright (c) 2015, Linaro Limited. All rights reserved.
//
//  SPDX-License-Identifier: BSD-2-Clause-Patent
//
//

#include "../Base.h"

.text
.align 5

GCC_ASM_EXPORT(CpuPause)

#/**
#
#  Requests CPU to pause for a short period of time.
#
#  Requests CPU to pause for a short period of time. Typically used in MP
#  systems to prevent memory starvation while waiting for a spin lock.
#
#**/
#VOID
#EFIAPI
#CpuPause (
#  VOID
#  )
#
ASM_PFX(CpuPause):
    nop
    nop
    nop
    nop
    nop
    ret
