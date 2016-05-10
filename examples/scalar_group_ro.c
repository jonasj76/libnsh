/* libnsh example -- Groups. Use one callback for several OIDs.
 *
 * This example will register two OIDs, 1.3.6.1.4.1.8072.9999.9999.1
 * and 1.3.6.1.4.1.8072.9999.9999.2.
 * .1 will return an integer value, and .2 will return a string.
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
