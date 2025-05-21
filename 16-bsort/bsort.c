/*
 * Copyright 2015 Eistec AB
 * Copyright 2021 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_checksum_fletcher32
 * @{
 *
 * @file
 * @brief       Fletcher32 implementation for rBPF
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * Implementation is adapted from our own in-tree application
 *
 * @}
 */

#include <stddef.h>

#include "shared.h"
#include "unaligned.h"


typedef struct bsort_context_s
{
    int size;
    __bpf_shared_ptr(int*, arr);
} bsort_context_t;


void bubblesort(bsort_context_t* ctx) {
  int size = ctx->size;
  //int * arr = ctx->arr;
  int i, j, tmp;
  for (i = 0;  i < size-1; i++) {
    for (j = 0; j < size - i-1; j++) {
      if (ctx->arr[j] > ctx->arr[j+1]) {
        tmp = ctx->arr[j];
        ctx->arr[j] = ctx->arr[j+1];
        ctx->arr[j+1] = tmp;
      }
    }
  }
}
