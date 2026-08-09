/*******************************************************************************/
/*  © Université de Lille, The Pip Development Team (2015-2026)                */
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
#include <limits.h>

#include "stdriot.h"

#define QUOTATION_MARK      "\""
#define QUOTATION_MARK_LEN  (sizeof(QUOTATION_MARK) - 1)

static void usage() {
    printf("scribe-echo.fae [ARGS]\n");
}

static char buffer[128] = { "\0" };

int main(int argc, char **argv)
{
    if (argc < 1) {
        usage();
        return -1;
    }

    int res = 0;
    scribe_code_t scribe_res = scribe_write(QUOTATION_MARK, QUOTATION_MARK_LEN);
    if (scribe_res != SCRIBE_CODE_OK) {
        printf("Failed to write to scribe \"%s\": %s (%d)\n",
               QUOTATION_MARK, scribe_code_get_label(scribe_res), scribe_res);
        return -1;
    }

    const char *separator = "";
    for (int i = 1; i < argc; i++) {

        res = snprintf(buffer, sizeof(buffer), "%s%s", separator, argv[i]);
        if ((res < 0) || (res >= (int)sizeof(buffer))) {
            printf("Failed to snprintf \"%s\": %d\n", argv[i], res);
            res = -1;
            goto exit;
        }

        scribe_res = scribe_write(buffer, res);
        if (scribe_res != SCRIBE_CODE_OK) {
            printf("Failed to write to scribe \"%s\": %s (%d)\n",
                   buffer, scribe_code_get_label(scribe_res), scribe_res);
            res = -1;
            goto exit;
        }

        separator = " ";
    }

    res = 0;
exit :
    scribe_res = scribe_write(QUOTATION_MARK, QUOTATION_MARK_LEN);
    if (scribe_res != SCRIBE_CODE_OK) {
        printf("Failed to write to scribe \"%s\": %s (%d)\n",
                QUOTATION_MARK, scribe_code_get_label(scribe_res), scribe_res);
        if (res >= 0) {
            res = -1;
        }
    }

    return res;
}
