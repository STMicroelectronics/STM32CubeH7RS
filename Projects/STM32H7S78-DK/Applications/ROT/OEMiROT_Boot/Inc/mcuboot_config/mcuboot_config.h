/*
 * Copyright (c) 2018 Open Source Foundries Limited
 * Copyright (c) 2019 Arm Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Original code taken from mcuboot project at:
 * https://github.com/JuulLabs-OSS/mcuboot
 * Git SHA of the original version: ac55554059147fff718015be9f4bd3108123f50a
 */

#ifndef __MCUBOOT_CONFIG_H__
#define __MCUBOOT_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This file is also included by the simulator, but we don't want to
 * define anything here in simulator builds.
 *
 * Instead of using mcuboot_config.h, the simulator adds MCUBOOT_xxx
 * configuration flags to the compiler command lines based on the
 * values of environment variables. However, the file still must
 * exist, or bootutil won't build.
 */
#ifndef __BOOTSIM__
/* FIH config */
#define MCUBOOT_FIH_PROFILE_HIGH    /* in this config random delay is activated at each FIH_CALL */
/*
 * With some implementations, most of the configuration flags (e.g. signature type,
 * upgrade mode ...) are handled by the CMake-based buildsystem and
 * added to the compiler command lines.
 */

#define MCUBOOT_FLASH_HOMOGENOUS     /* Defined: flash having the same sector size can use this flag */

/* Available crypto schemes (do not change values, as used in appli postbuild script) */
#define CRYPTO_SCHEME_EC256      0x2 /* ECDSA-256 signature,
                                        AES-CTR-128 encryption with key ECIES-P256 encrypted */

/* Crypto scheme selection : begin */
#define CRYPTO_SCHEME            CRYPTO_SCHEME_EC256  /* Select one of available crypto schemes */
/* Crypto scheme selection : end */


/* ECC config */
#define NUM_ECC_BYTES 32
#define MCUBOOT_SIGN_EC256
#define MCUBOOT_ENCRYPT_EC256

#define MCUBOOT_VALIDATE_PRIMARY_SLOT
#define MCUBOOT_USE_FLASH_AREA_GET_SECTORS

#define MCUBOOT_HW_ROLLBACK_PROT
#define MCUBOOT_ENC_IMAGES
#define MCUBOOT_BOOTSTRAP            /* Allow initial state with images in secondary slots only (empty primary slots) */
#define MCUBOOT_ENC_SECURITY_CNT     /* Security counters are encrypted: resets of boot_loader_state->enc are needed */

/*
 * Cryptographic settings
 */
#define MCUBOOT_USE_HAL

#if defined(MCUBOOT_USE_HAL)
#include "stm32h7rsxx_hal.h"
#define MCUBOOT_USE_HASH_HAL_V2
#define PKA_ECDSA_SIGNATURE_ADDRESS 0x0578UL
#endif /* MCUBOOT_USE_HAL */

/*
 * Logging
 */
#ifdef OEMIROT_DEV_MODE
#define MCUBOOT_HAVE_LOGGING    1
#endif 

#endif /* !__BOOTSIM__ */

/*
 * Watchdog feeding
 */
#define MCUBOOT_WATCHDOG_FEED()     \
    do {                            \
        /* Do nothing. */           \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* __MCUBOOT_CONFIG_H__ */
