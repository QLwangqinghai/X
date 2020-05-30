/* Copyright (c) 2018, Google Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. */

#include "XCrypto_cpu.h"

#if defined(OPENSSL_AARCH64) && defined(XCRYPTO_FUCHSIA) && \
    !defined(OPENSSL_STATIC_ARMCAP)

#include <zircon/features.h>
#include <zircon/syscalls.h>
#include <zircon/types.h>

#include "XCrypto_arm_arch.h"

#include "internal.h"

extern uint32_t XCRYPTO_ARMcap_P;

void OPENSSL_cpuid_setup(void) {
  uint32_t hwcap;
  zx_status_t rc = zx_system_get_features(ZX_FEATURE_KIND_CPU, &hwcap);
  if (rc != ZX_OK || (hwcap & ZX_ARM64_FEATURE_ISA_ASIMD) == 0) {
    // Matching OpenSSL, if NEON/ASIMD is missing, don't report other features
    // either.
    return;
  }

  XCRYPTO_ARMcap_P |= ARMV7_NEON;

  if (hwcap & ZX_ARM64_FEATURE_ISA_AES) {
    XCRYPTO_ARMcap_P |= ARMV8_AES;
  }
  if (hwcap & ZX_ARM64_FEATURE_ISA_PMULL) {
    XCRYPTO_ARMcap_P |= ARMV8_PMULL;
  }
  if (hwcap & ZX_ARM64_FEATURE_ISA_SHA1) {
    XCRYPTO_ARMcap_P |= ARMV8_SHA1;
  }
  if (hwcap & ZX_ARM64_FEATURE_ISA_SHA2) {
    XCRYPTO_ARMcap_P |= ARMV8_SHA256;
  }
}


#endif  // OPENSSL_AARCH64 && !OPENSSL_STATIC_ARMCAP
