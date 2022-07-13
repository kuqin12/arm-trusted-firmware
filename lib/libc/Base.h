/** @file AdvancedLoggerInternal.h

    Advanced Logger internal data structures


    Copyright (C) Microsoft Corporation. All rights reserved.
    SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef UEFI_BASE_H_
#define UEFI_BASE_H_

#ifndef __USER_LABEL_PREFIX__
#define __USER_LABEL_PREFIX__
#endif

///
/// Private worker functions for ASM_PFX()
///
#define _CONCATENATE(a, b)   __CONCATENATE(a, b)
#define __CONCATENATE(a, b)  a ## b

///
/// The __USER_LABEL_PREFIX__ macro predefined by GNUC represents the prefix
/// on symbols in assembly language.
///
#define ASM_PFX(name)  _CONCATENATE (__USER_LABEL_PREFIX__, name)

#define GCC_ASM_EXPORT(func__)  \
         .global  _CONCATENATE (__USER_LABEL_PREFIX__, func__)    ;\
         .type ASM_PFX(func__), %function


#define _ASM_FUNC(Name, Section)    \
  .global   Name                  ; \
  .section  #Section, "ax"        ; \
  .type     Name, %function       ; \
  Name:

#define ASM_FUNC(Name)  _ASM_FUNC(ASM_PFX(Name), .text. ## Name)

#endif // UEFI_BASE_H_
