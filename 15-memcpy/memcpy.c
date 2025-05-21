/*
 * Copyright 2015 Eistec AB
 * Copyright 2021 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup
 * @{
 *
 * @file
 *
 * @}
 */


#include<stdint.h>

#include "shared.h"
typedef struct memcpy_ctx_s
{
    __bpf_shared_ptr(char*, src);
    __bpf_shared_ptr(char*, dst);
    uint32_t len;
} memcpy_ctx_t;

uint32_t mempcy(memcpy_ctx_t *ctx)
{
    char* src = ctx->src;
    char* dst = ctx->dst;
    uint32_t len = ctx->len;
    for (uint32_t i = 0; i < len; i++) {
        dst[i] = src[i];
  }
  return 0;
}

