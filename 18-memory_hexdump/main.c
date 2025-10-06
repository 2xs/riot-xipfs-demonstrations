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
#include <inttypes.h>

#include "stdriot.h"

extern uint32_t __rom_start;

extern uint32_t __ram_start;

static uint32_t own_strlen(const char *str) {
    uint32_t len = 0;

    if (str != NULL) {
        while (str[len] !='\0')
            ++len;
    }

    return len;
}

static int own_strcmp(const char *a, const char *b) {
    if (a == NULL)
        return (b == NULL) ? 0 : -1;
    if (b == NULL)
        return 1;
    const uint32_t a_len = own_strlen(a);
    const uint32_t b_len = own_strlen(b);
    if (a_len < b_len)
        return -1;
    if (a_len > b_len)
        return 1;
    for (uint32_t i = 0; i < a_len; ++i) {
        if (a[i] < b[i])
            return -1;
        if (a[i] > b[i])
            return 1;
    }

    return 0;
}

#define FIRST_COLUMN_END 8
#define SECOND_COLUMN_END 16

static void dump(const void *address) {
    const unsigned char *pointer, *end_address;
    unsigned char byte;
    unsigned int i, j;

    pointer = address;
    end_address = (void *)(uintptr_t)pointer + 64;
    while (pointer < end_address) {
        printf("%02" PRIxPTR "  ", (uintptr_t)pointer);

        i = 0;
        while (i < FIRST_COLUMN_END && pointer + i < end_address)
            printf("%02x ", pointer[i++]);

        printf("  ");

        while (i < SECOND_COLUMN_END && pointer + i < end_address)
            printf("%02x ", pointer[i++]);

        j = i;

        while (i < SECOND_COLUMN_END) {
            printf("   ");
            i++;
        }

        printf(" |");

        i = 0;
        while (i < j) {
            byte = pointer[i];
            if (isprint(byte) != 0)
                printf("%c", byte);
            else
                printf(".");
            i++;
        }

        printf("|\n");

        pointer += i;
    }

    printf("%" PRIxPTR "\n", (uintptr_t)pointer);
}

static void usage() {
    printf("dumper.fae {legit-ram, non-legit-ram, legit-rom, non-legit-rom} \n");
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        usage();
        return 1;
    }

    if (own_strcmp(argv[1], "legit-ram") == 0) {
        dump((const char *)(&__ram_start));
        return 0;
    }

    if (own_strcmp(argv[1], "non-legit-ram") == 0) {
        uintptr_t start = 0x20000020;
        dump((const char *)start);
        return 0;
    }

    if (own_strcmp(argv[1], "legit-rom") == 0) {
        dump((const char *)(&__rom_start));
        return 0;
    }

    if (own_strcmp(argv[1], "non-legit-rom") == 0) {
        uintptr_t start = 0x0;
        dump((const char *)start);
        return 0;
    }
    usage();

    return 1;
}
