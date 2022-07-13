#------------------------------------------------------------------------------
#
# Copyright (c) 2011 - 2013, ARM Limited. All rights reserved.
# Copyright (c) 2016, Linaro Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
#------------------------------------------------------------------------------

#include "../Base.h"

ASM_FUNC(ArmReadCntPct)
  mrs   x0, cntpct_el0           // Read CNTPCT (Physical counter register)
  ret

ASM_FUNC(ArmReadCntFrq)
  mrs   x0, cntfrq_el0           // Read CNTFRQ
  ret
