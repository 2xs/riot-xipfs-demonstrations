/*******************************************************************************/
/*  © Université de Lille, The Pip Development Team (2015-2024)                */
/*                                                                             */
/*  This software is a computer program whose purpose is to run a minimal,     */
/*  hypervisor relying on proven properties such as memory isolation.          */
/*                                                                             */
/*  This software is governed by the CeCILL license under French law and       */
/*  abiding by the rules of distribution of free software.  You can  use,      */
/*  modify and/ or redistribute the software under the terms of the CeCILL     */
/*  license as circulated by CEA, CNRS and INRIA at the following URL          */
/*  "http://www.cecill.info".                                                  */
/*                                                                             */
/*  As a counterpart to the access to the source code and  rights to copy,     */
/*  modify and redistribute granted by the license, users are provided only    */
/*  with a limited warranty  and the software's author,  the holder of the     */
/*  economic rights,  and the successive licensors  have only  limited         */
/*  liability.                                                                 */
/*                                                                             */
/*  In this respect, the user's attention is drawn to the risks associated     */
/*  with loading,  using,  modifying and/or developing or reproducing the      */
/*  software by the user in light of its specific status of free software,     */
/*  that may mean  that it is complicated to manipulate,  and  that  also      */
/*  therefore means  that it is reserved for developers  and  experienced      */
/*  professionals having in-depth computer knowledge. Users are therefore      */
/*  encouraged to load and test the software's suitability as regards their    */
/*  requirements in conditions enabling the security of their systems and/or   */
/*  data to be ensured and,  more generally, to use and operate it in the      */
/*  same conditions as regards security.                                       */
/*                                                                             */
/*  The fact that you are presently reading this means that you have had       */
/*  knowledge of the CeCILL license and that you accept its terms.             */
/*******************************************************************************/

#include <assert.h>
#include <inttypes.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>

#include "rbpf.h"
#include "shared.h"
#include "stdriot.h"

#define PROGNAME "rbpf-unsafe-bench.fae"

#define RBPF_STACK_SIZE   (512)
#define BYTECODE_SIZE_MAX (600)
#define BUFFER_SIZE_MAX   (362)

static uint8_t rbpf_stack[RBPF_STACK_SIZE];
static uint8_t buf[BUFFER_SIZE_MAX];
static uint8_t bytecode[BYTECODE_SIZE_MAX];


#define BPF_RUN_N(ctx, size) \
    do { \
        status = RBPF_OK; \
        for (i = 0; i < n && status == RBPF_OK; i++) { \
            status = rbpf_application_run_ctx(rbpf, ctx, \
                size, &result); \
        } \
    } while (0)

static int
bpf_print_result(int64_t result, int status)
{
    switch (status) {
    case RBPF_OK:
        printf(PROGNAME": %lx\n", (uint32_t)result);
        return 0;
    case RBPF_ILLEGAL_MEM:
        printf(PROGNAME": illegal memory access\n");
        return 1;
    case RBPF_ILLEGAL_INSTRUCTION :   /**< Failed on instruction parsing */
        printf(PROGNAME": illegal instruction\n");
        return 1;
    case RBPF_ILLEGAL_JUMP :
        printf(PROGNAME": illegal jump\n");
        return 1;
    case RBPF_ILLEGAL_CALL :
        printf(PROGNAME": illegal call\n");
        return 1;
    case RBPF_ILLEGAL_LEN :
        printf(PROGNAME": illegal len\n");
        return 1;
    case RBPF_ILLEGAL_REGISTER :
        printf(PROGNAME": illegal register\n");
        return 1;
    case RBPF_NO_RETURN :
        printf(PROGNAME": no return\n");
        return 1;
    case RBPF_OUT_OF_BRANCHES :
        printf(PROGNAME": out of branches\n");
        return 1;
    case RBPF_ILLEGAL_DIV :
        printf(PROGNAME": illegal div\n");
        return 1;
    default:
        printf(PROGNAME": error\n");
        return 1;
    }
}

static int bpf_run_with_context(rbpf_application_t *rbpf, unsigned n, void *context,
                                size_t context_size) {
    int64_t result = 0;
    int status;
    unsigned i;

    BPF_RUN_N(context, context_size);

    return bpf_print_result(result, status);
}

static int
bpf_run_with_integer(rbpf_application_t *rbpf, unsigned n, uint64_t integer)
{
    int64_t result;
    int status;
    unsigned i;

    BPF_RUN_N(&integer, sizeof(integer));

    return bpf_print_result(result, status);
}


typedef enum bench_cases_e {
    BENCH_CASE_INCR = 0,
    BENCH_CASE_SQUARE,
    BENCH_CASE_FIBONACCI,
    BENCH_CASE_BITSWAP,
    BENCH_CASE_ARITHMETIC_FIRST = BENCH_CASE_INCR,
    BENCH_CASE_ARITHMETIC_LAST = BENCH_CASE_BITSWAP,

    BENCH_CASE_FLETCHER32,
    BENCH_CASE_SOCKBUF,
    BENCH_CASE_MEMCPY,
    BENCH_CASE_BUBBLE_SORT,

    BENCH_CASE_FIRST = BENCH_CASE_ARITHMETIC_FIRST,
    BENCH_CASE_LAST  = BENCH_CASE_BUBBLE_SORT
} bench_cases_t;

#define BENCH_CASES_COUNT ((BENCH_CASE_LAST - BENCH_CASE_FIRST) + 1)

typedef struct bench_case_info_s {
    const char *name;
    const char *filename;
    const char *help_arguments;
} bench_case_info_t;

#define BENCH_CASE_INFO_INIT(bench_case_name, bench_case_filename, bench_case_help_arguments) \
{ \
    .name           = bench_case_name, \
    .filename       = bench_case_filename, \
    .help_arguments = bench_case_help_arguments \
}

#define DIRECTORY    "/nvme0p1/"

static const bench_case_info_t bench_case_infos[BENCH_CASES_COUNT] = {
    [       BENCH_CASE_INCR] = BENCH_CASE_INFO_INIT("incr", DIRECTORY "incr.rbpf", "uint32_t"),
    [     BENCH_CASE_SQUARE] = BENCH_CASE_INFO_INIT("square", DIRECTORY "square.rbpf", "uint32_t"),
    [  BENCH_CASE_FIBONACCI] = BENCH_CASE_INFO_INIT("fibonacci", DIRECTORY "fibonacci.rbpf", "uint32_t"),
    [    BENCH_CASE_BITSWAP] = BENCH_CASE_INFO_INIT("bitswap", DIRECTORY "bitswap.rbpf", ""),

    [ BENCH_CASE_FLETCHER32] = BENCH_CASE_INFO_INIT("fletcher32", DIRECTORY "fletcher32.rbpf", "filename"),
    [    BENCH_CASE_SOCKBUF] = BENCH_CASE_INFO_INIT("sockbuf", DIRECTORY "sockbuf.rbpf", ""),
    [     BENCH_CASE_MEMCPY] = BENCH_CASE_INFO_INIT("memcpy", DIRECTORY "memcpy.rbpf", ""),
    [BENCH_CASE_BUBBLE_SORT] = BENCH_CASE_INFO_INIT("bubble_sort", DIRECTORY "bsort.rbpf", ""),
};

static void usage(void) {
    printf(PROGNAME": RUNS_COUNT BENCH_CASE_ID [ARGUMENTS] where BENCH_CASE_ID is :\n");

    const char *separator = "";
    for(int i = BENCH_CASE_FIRST; i < BENCH_CASES_COUNT; ++i) {
        const bench_case_info_t *bench_case_info = &(bench_case_infos[i]);
        printf("%s\t- %d, aka %s", separator , i, bench_case_info->name);
        if (bench_case_info->help_arguments[0] != '\0')
            printf(", argument(s) : %s", bench_case_info->help_arguments);
        separator = "\n";
    }
    printf("\n");
}

static int bpf_load_file_to_buffer(const char *filename) {
    int result = copy_file(filename, buf, BUFFER_SIZE_MAX);
    if (result < 0) {
        printf(PROGNAME ": failed to load data from file \"%s\".\n", filename);
    } else
        printf(PROGNAME": \"%s\" data loaded at address %p\n", filename, (void *)buf);

    return result;
}

int init_rbpf(rbpf_application_t *rbpf, const char *bytecode_filename) {
    int result;
    size_t bytecode_size;
    rbpf_mem_region_t region;

    if ((result = copy_file(bytecode_filename, bytecode, BYTECODE_SIZE_MAX)) < 0) {
        printf(PROGNAME": %s: failed to copy bytecode\n", bytecode_filename);
        return 1;
    }
    bytecode_size = (size_t)result;

    printf(PROGNAME": \"%s\" bytecode loaded at address %p\n", bytecode_filename,
        (void *)bytecode);

    rbpf_application_setup(rbpf, rbpf_stack, (void *)bytecode, bytecode_size);
    rbpf_memory_region_init(&region, bytecode, bytecode_size,
        RBPF_MEM_REGION_READ);
    rbpf_add_region(rbpf, &region);

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
typedef struct {
    __bpf_shared_ptr(const uint16_t *, data);
    uint32_t words;
} fletcher32_ctx_t;

static int bpf_run_fletcher32(rbpf_application_t *rbpf,unsigned n, int argc, const char *argv[]) {
    fletcher32_ctx_t ctx;
    rbpf_mem_region_t region;
    int buf_size;

    if (argc < 4) {
        usage();
        return 1;
    }

    buf_size = bpf_load_file_to_buffer(argv[3]);
    if (buf_size <= 0)
        return 1;

    ctx.data = (const uint16_t *)buf;
    ctx.words = buf_size/2;

    rbpf_memory_region_init(&region, buf, buf_size, RBPF_MEM_REGION_READ);
    rbpf_add_region(rbpf, &region);

    return bpf_run_with_context(rbpf, n, (void *)&ctx, sizeof(ctx));
}

///////////////////////////////////////////////////////////////////////////////
typedef struct bitswap_ctx_s{
    uint8_t value;
    uint8_t bit1;
    uint8_t bit2;
}bitswap_ctx_t;

static int bpf_run_bitswap(rbpf_application_t *rbpf,unsigned n, int argc, const char *argv[]) {
    if (argc != 3) {
        usage();
        return 1;
    }

    bitswap_ctx_t ctx;
    ctx.value = 42;
    ctx.bit1  = 2;
    ctx.bit2  = 3;

    return bpf_run_with_context(rbpf, n, (void *)&ctx, sizeof(ctx));
}

///////////////////////////////////////////////////////////////////////////////
#define ARRAY_LENGTH 40

uint32_t sockbuf_array[ARRAY_LENGTH];

typedef struct sockbuf_ctx_s
{
    uint32_t data_start;
    uint32_t data_end;
    uint32_t len;
    __bpf_shared_ptr(uint32_t *, array);
} sockbuf_ctx_t;

static int bpf_run_sockbuf(rbpf_application_t *rbpf,unsigned n, int argc, const char *argv[]) {
    rbpf_mem_region_t region;

    if (argc != 3) {
        usage();
        return 1;
    }

    for(unsigned int i = 0; i < ARRAY_LENGTH; ++i) {
        sockbuf_array[i] = 0;
    }

    sockbuf_ctx_t ctx = {
        .data_start = 100,
        .data_end   = 200,
        .len        = 9,
        .array      = sockbuf_array
    };

    rbpf_memory_region_init(&region, sockbuf_array, sizeof(sockbuf_array), RBPF_MEM_REGION_READ | RBPF_MEM_REGION_WRITE);
    rbpf_add_region(rbpf, &region);

    return bpf_run_with_context(rbpf, n, (void *)&ctx, sizeof(ctx));
}

///////////////////////////////////////////////////////////////////////////////
char dst_data[60];

typedef struct memcpy_ctx_s
{
    __bpf_shared_ptr(char*, src);
    __bpf_shared_ptr(char*, dst);
    uint32_t len;
} memcpy_ctx_t;

static int bpf_run_memcpy(rbpf_application_t *rbpf, unsigned n, int argc, const char *argv[]) {
    (void)argv;
    if(argc != 3) {
        usage();
        return 1;
    }

#define MEMCPY_DATA_FILENAME "/nvme0p1/memcpy_data.dta"
    int data_loading_result = bpf_load_file_to_buffer(MEMCPY_DATA_FILENAME);
    if (data_loading_result < 0)
        return 1;

    memcpy_ctx_t ctx = {
        .len = (uint32_t)sizeof(dst_data),
        .src = (char *)buf,
        .dst = dst_data
    };

    rbpf_mem_region_t src_region;
    rbpf_memory_region_init(&src_region, buf, data_loading_result, RBPF_MEM_REGION_READ);
    rbpf_add_region(rbpf, &src_region);

    rbpf_mem_region_t dst_region;
    rbpf_memory_region_init(&dst_region, dst_data, sizeof(dst_data), RBPF_MEM_REGION_WRITE);
    rbpf_add_region(rbpf, &dst_region);

    return bpf_run_with_context(rbpf, n, &ctx, sizeof(memcpy_ctx_t));
}

///////////////////////////////////////////////////////////////////////////////
typedef struct bsort_context_s
{
    int size;
    __bpf_shared_ptr(int*, arr);
} bsort_context_t;

static int bpf_run_bubble_sort(rbpf_application_t *rbpf,unsigned n, int argc, const char *argv[]) {
    if(argc != 3) {
        usage();
        return 1;
    }

#define BSORT_DATA_FILENAME "/nvme0p1/bsort_data.dta"
    int data_loading_result = bpf_load_file_to_buffer(BSORT_DATA_FILENAME);
    if (data_loading_result < 0)
        return 1;

    bsort_context_t ctx = {
        .size = data_loading_result / 4,
        .arr  = (int *)buf
    };

    rbpf_mem_region_t region;
    rbpf_memory_region_init(&region, buf, data_loading_result, RBPF_MEM_REGION_READ | RBPF_MEM_REGION_WRITE);
    rbpf_add_region(rbpf, &region);

    return bpf_run_with_context(rbpf, n, &ctx, sizeof(bsort_context_t));
}


int
main(int argc, const char *argv[])
{
    rbpf_application_t rbpf;
    uint64_t integer;
    char *endptr;
    unsigned n, bench_case_id;
    int ret;

    if (argc < 3) {
        usage();
        return 1;
    }

    n = (unsigned)strtol(argv[1], &endptr, 10);
    if (argv[1] == endptr ||  *endptr != '\0') {
        printf(PROGNAME": %s: failed to parse RUNS_COUNT\n", argv[1]);
        return 1;
    }

    bench_case_id = (unsigned)strtol(argv[2], &endptr, 10);
    if (argv[2] == endptr ||  *endptr != '\0') {
        printf(PROGNAME": %s: failed to parse bench case\n", argv[2]);
        return 1;
    }

    switch(bench_case_id) {
        case BENCH_CASE_INCR :
        /* FALLTHROUGH */
        case BENCH_CASE_SQUARE :
        /* FALLTHROUGH */
        case BENCH_CASE_FIBONACCI : {
            if (argc < 4) {
                usage();
                return 1;
            }

            ret = init_rbpf(&rbpf, bench_case_infos[bench_case_id].filename);
            if (ret != 0)
                return ret;

            integer = (unsigned)strtol(argv[3], &endptr, 10);
            if (argv[3] == endptr ||  *endptr != '\0') {
                printf(PROGNAME": %s: failed to parse integer\n", argv[3]);
                return 1;
            }
            return bpf_run_with_integer(&rbpf, n, integer);
        }

        case BENCH_CASE_FLETCHER32 : {
            ret = init_rbpf(&rbpf, bench_case_infos[bench_case_id].filename);
            if (ret != 0)
                return ret;
            return bpf_run_fletcher32(&rbpf, n, argc, argv);
        }
        case BENCH_CASE_BITSWAP : {
            ret = init_rbpf(&rbpf, bench_case_infos[bench_case_id].filename);
            if (ret != 0)
                return ret;
            return bpf_run_bitswap(&rbpf, n, argc, argv);
        }
        case BENCH_CASE_SOCKBUF : {
            ret = init_rbpf(&rbpf, bench_case_infos[bench_case_id].filename);
            if (ret != 0)
                return ret;
            return bpf_run_sockbuf(&rbpf, n, argc, argv);
        }
        case BENCH_CASE_MEMCPY : {
            ret = init_rbpf(&rbpf, bench_case_infos[bench_case_id].filename);
            if (ret != 0)
                return ret;
            return bpf_run_memcpy(&rbpf, n, argc, argv);
        }
        case BENCH_CASE_BUBBLE_SORT : {
            ret = init_rbpf(&rbpf, bench_case_infos[bench_case_id].filename);
            if (ret != 0)
                return ret;
            return bpf_run_bubble_sort(&rbpf, n, argc, argv);
        }
        default :
            usage();
            return 1;
    }

    /* other argument types are not yet supported */

    return 1;
}
