/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * Original code taken from mcuboot project at:
 * https://github.com/JuulLabs-OSS/mcuboot
 * Git SHA of the original version: ac55554059147fff718015be9f4bd3108123f50a
 * Modifications are Copyright (c) 2019 Arm Limited.
 *                   Copyright (c) 2023 STMicroelectronics.
 */

#include <bootutil/sign_key.h>
#include "mcuboot_config/mcuboot_config.h"
#include "flash_layout.h"
#include "mbedtls_config.h"
#include "low_level_obkeys.h"

#if defined(MCUBOOT_SIGN_RSA)
#if MCUBOOT_SIGN_RSA_LEN == 2048
const unsigned int rsa2048_pub_key_len  = 270;
const struct bootutil_key bootutil_keys[] =
{
  {
    .key = OBK_Hdpl1Cfg.Hdpl3SecureAuthenticationPubKey,
    .len = &rsa2048_pub_key_len,
  },
};
#else /* not (MCUBOOT_SIGN_RSA_LEN == 2048) */
extern const unsigned int rsa3072_pub_key_len;
extern const unsigned char rsa3072_pub_key[];
const struct bootutil_key bootutil_keys[] =
{
  {
    .key = OBK_Hdpl1Cfg.Hdpl3SecureAuthenticationPubKey,
    .len = &rsa3072_pub_key_len,
  },
};
#endif /* MCUBOOT_SIGN_RSA_LEN == 2048 */
#elif defined(MCUBOOT_SIGN_EC256)
const unsigned int ecdsa_pub_key_len = 91;
const struct bootutil_key bootutil_keys[] =
{
  {
#if  defined(OEMUROT_ENABLE)
    .key = OBK_Hdpl2Cfg.Hdpl3AuthenticationPubKey,
#else /* not OEMUROT_ENABLE */
    .key = OBK_Hdpl1Cfg.Hdpl3AuthenticationPubKey,
#endif /* OEMUROT_ENABLE */
    .len = &ecdsa_pub_key_len,
  },
};
#else /* not MCUBOOT_SIGN_RSA and not MCUBOOT_SIGN_EC256 */
#error "No public key available for given signing algorithm."
#endif /* MCUBOOT_SIGN_RSA */
const int bootutil_key_cnt = MCUBOOT_IMAGE_NUMBER;
#if defined(MCUBOOT_ENC_IMAGES)
#if defined(MCUBOOT_ENCRYPT_RSA)
const unsigned int enc_rsa_priv_key_len = 1192;
const struct bootutil_key bootutil_enc_key =
{
  .key = OBK_Hdpl1Cfg.Hdpl3EncryptionPrivKey,
  .len = &enc_rsa_priv_key_len,
};
#elif defined(MCUBOOT_ENCRYPT_EC256)

const unsigned int enc_ec256_priv_key_len = 70;
const struct bootutil_key bootutil_enc_key =
{
#if  defined(OEMUROT_ENABLE)
  .key = OBK_Hdpl2Cfg.Hdpl3EncryptionPrivKey,
#else /* not OEMUROT_ENABLE */
  .key = OBK_Hdpl1Cfg.Hdpl3EncryptionPrivKey,
#endif /* OEMUROT_ENABLE */
  .len = &enc_ec256_priv_key_len,
};
#endif /* MCUBOOT_ENCRYPT_RSA */
#else /* not MCUBOOT_ENC_IMAGES */
const struct bootutil_key bootutil_enc_key =
{
  .key = (const unsigned char *)0,
  .len = (const unsigned int *)0,
};
#endif /* MCUBOOT_ENC_IMAGES */
