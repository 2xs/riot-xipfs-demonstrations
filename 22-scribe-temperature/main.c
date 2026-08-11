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

#if defined(TEMPERATURE_SCALE_CELSIUS) && defined(TEMPERATURE_SCALE_KELVIN)
#error "Both Celsius and Kelvin scales are defined"
#endif /* defined(TEMPERATURE_SCALE_CELSIUS) && defined(TEMPERATURE_SCALE_KELVIN) */

#if (!defined(TEMPERATURE_SCALE_CELSIUS)) && (!defined(TEMPERATURE_SCALE_KELVIN))
#define TEMPERATURE_SCALE_CELSIUS
#endif

#ifdef TEMPERATURE_SCALE_CELSIUS
#define UNIT_LABEL "C"
#endif

#ifdef TEMPERATURE_SCALE_KELVIN
#define UNIT_LABEL "K"
#endif

#define SCRIBE_MESSAGE_0     "{\"temp\":{\"v\":"
#define SCRIBE_MESSAGE_0_LEN (sizeof(SCRIBE_MESSAGE_0) - 1)

#define SCRIBE_MESSAGE_1     ",\"u\":\"" UNIT_LABEL "\"}}"
#define SCRIBE_MESSAGE_1_LEN (sizeof(SCRIBE_MESSAGE_1) - 1)

static const char *scribe_message[2] =
{
    SCRIBE_MESSAGE_0,
    SCRIBE_MESSAGE_1
};

static const size_t scribe_message_len[2] =
{
    SCRIBE_MESSAGE_0_LEN,
    SCRIBE_MESSAGE_1_LEN
};

static char buffer[64];

static void usage() {
    printf("scribe-temperature.fae\n");
}

static int write(const char *text, size_t text_len)
{
    scribe_code_t scribe_res = scribe_write(text, text_len);
    if (scribe_res != SCRIBE_CODE_OK) {
        printf("Failed to write to scribe \"%s\": %s (%d)\n",
               text, scribe_code_get_label(scribe_res), scribe_res);
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 1) {
        usage();
        return -1;
    }

    int res  = write(scribe_message[0], scribe_message_len[0]);
    if (res < 0) {
        return res;
    }

    const int temp =
#ifdef TEMPERATURE_SCALE_KELVIN
        27315 +
#endif
        get_temp();

    res = snprintf(buffer, sizeof(buffer), "%d.%02d", temp / 100, temp % 100);
    if ((res < 0) < (res >= (int)sizeof(buffer))) {
        return -1;
    }

    res = write(buffer, res);
    if (res < 0) {
        return -1;
    }

    return write(scribe_message[1], scribe_message_len[1]);
}
