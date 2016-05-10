/* libnsh example -- Read-only OID
 *
 * This example will register an OID, 1.3.6.1.4.1.8072.9999.9999.1,
 * which will return an integer value when the OID is requested.
 */

#include <stdlib.h>
#include <time.h>

#include <nsh.h>

#include "agent.h"

#define oid_netSnmpPlaypen 1, 3, 6, 1, 4, 1, 8072, 9999, 9999
#define oid_exampleOid     oid_netSnmpPlaypen, 1

static int callback(void *value, int len, int id)
{
	*(long*)value = 10;

	return SNMP_ERR_NOERROR;
}

nsh_scalar_handler_ro(exampleOid, ASN_INTEGER, callback, sizeof(long));

int main(void)
{
	int err;

	/* Init SNMP subagent */
	agent_init();

	/* Register OID */
	err = nsh_register_scalar_ro(exampleOid);
	if (err) {
		fprintf(stderr, "Error registering OID\n");
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
