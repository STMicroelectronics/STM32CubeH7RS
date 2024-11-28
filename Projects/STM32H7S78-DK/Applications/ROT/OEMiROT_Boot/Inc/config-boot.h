/*
 *  Minimal configuration for using TLS in the bootloader
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Copyright (C) 2016, Linaro Ltd
 *  Copyright (c) 2023 STMicroelectronics
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

/*
 * Minimal configuration for using TLS in the bootloader
 *
 * - RSA or ECDSA signature verification
 */

#ifndef MBEDTLS_CONFIG_BOOT_H
#define MBEDTLS_CONFIG_BOOT_H

#ifdef __cplusplus
extern "C" {
#endif


#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
/* HW accelerators activation in BL2 */
#define BL2_HW_ACCEL_ENABLE

/* mbedtls-asn1 modules */
#define MBEDTLS_ASN1_PARSE_C
#else
#define MBEDTLS_ECP_NO_INTERNAL_RNG

/* System support */
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C


#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
 /* Changes for FIH profile HIGH */
#undef MBEDTLS_TEST_NULL_ENTROPY
#undef MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
#define MBEDTLS_ECP_NIST_OPTIM                        /* undefine this macro to disable NIST curves optimisation */
#define MBEDTLS_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_HMAC_DRBG_C


/* STD functions */
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS

#define MBEDTLS_PLATFORM_EXIT_ALT
#define MBEDTLS_PLATFORM_PRINTF_ALT
#define MBEDTLS_PLATFORM_SNPRINTF_ALT

#if !defined(CONFIG_ARM)
#define MBEDTLS_HAVE_ASM
#endif

#define MBEDTLS_RSA_C
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_C
#define MBEDTLS_ECDH_C
#define MBEDTLS_ECDSA_C
#define MBEDTLS_PKCS1_V21
#define MBEDTLS_HKDF_C


/* mbed TLS modules */
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_MD_C
#define MBEDTLS_OID_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_CIPHER_MODE_CTR
#define MBEDTLS_AES_C
#define MBEDTLS_CIPHER_C
/* Save RAM by adjusting to our exact needs */
#define MBEDTLS_ECP_MAX_BITS             2048


#define MBEDTLS_MPI_MAX_SIZE              384


#define MBEDTLS_SSL_MAX_CONTENT_LEN 1024

/* Save ROM and a few bytes of RAM by specifying our own ciphersuite list */
#define MBEDTLS_SSL_CIPHERSUITES MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8


#include "mbedtls/check_config.h"
#endif /* STM32H7S3xx || STM32H7S7xx */



#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CONFIG_BOOT_H */
