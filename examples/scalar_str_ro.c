/* libnsh example -- String OID
 *
 * This example will register an OID, 1.3.6.1.4.1.8072.9999.9999.1,
 * which will return a string when the OID is requested.
 */

#include <stdio.h>

#include <nsh.h>

#include "agent.h"

#define oid_netSnmpPlaypen 1, 3, 6, 1, 4, 1, 8072, 9999, 9999
#define oid_exampleOid     oid_netSnmpPlaypen, 1

#define MAX_STRING_LENGTH 32

static int callback(void *value, int len, int id)
{
	snprintf((char*)value, len, "OID string");

	return SNMP_ERR_NOERROR;
}

nsh_scalar_str_handler_ro(exampleOid, callback, MAX_STRING_LENGTH);

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
