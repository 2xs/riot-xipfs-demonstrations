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


typedef struct sockbuf_ctx_s
{
    uint32_t data_start;
    uint32_t data_end;
    uint32_t len;
    __bpf_shared_ptr(uint32_t *, array);
} sockbuf_ctx_t;

uint32_t sockbuf(sockbuf_ctx_t *ctx)
{
    uint32_t* array     = ctx->array;
    uint32_t data_start = ctx->data_start;
    uint32_t data_end   = ctx->data_end;
    uint32_t len        = ctx->len;
    uint32_t index;
    uint32_t cumul = 0;


    for (index = 0U; index < len; index++) {
        if ((data_start + index) >= data_end)
            break;

        array[index] = 1U;
    }

    for (index = 0U; index < len; index++) {
        cumul += array[index];
    }
    return cumul;
}

