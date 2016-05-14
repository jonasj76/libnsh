/* libnsh example - Use one callback for several OIDs
 *
 * Copyright (c) 2016  Jonas Johansson <jonasj76@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include <time.h>

#include <nsh.h>

#include "agent.h"

#define oid_netSnmpPlaypen 1, 3, 6, 1, 4, 1, 8072, 9999, 9999
#define oid_exampleOidOne  oid_netSnmpPlaypen, 1
#define oid_exampleOidTwo  oid_netSnmpPlaypen, 2

#define MAX_STRING_LENGTH 32

#define ID_1 1
#define ID_2 2

static int callback(void *value, int len, int id)
{
	switch(id) {
	case ID_1:
		*(long*)value = 10;
		break;
	case ID_2:
		snprintf((char*)value, len, "OID string");
		break;
	}

	return SNMP_ERR_NOERROR;
}

nsh_scalar_group_handler_ro(exampleOidOne, ASN_INTEGER,   ID_1, callback, sizeof(long),      0);
nsh_scalar_group_handler_ro(exampleOidTwo, ASN_OCTET_STR, ID_2, callback, MAX_STRING_LENGTH, 1);

int main(void)
{
	int err = 0;

	/* Init SNMP subagent */
	agent_init();

	/* Register OIDs */
	err += nsh_register_scalar_ro(exampleOidOne);
	err += nsh_register_scalar_ro(exampleOidTwo);
	if (err) {
		fprintf(stderr, "Error registering OID(s)\n");
		return err;
	}

	/* Event loop to handle SNMP requests */
	agent_event_loop();
}

/**
 * Local Variables:
 *  c-file-style: "linux"
 * End:
 */
