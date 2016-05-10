/* libnsh example -- Constant OID
 *
 * This example will register an OID, 1.3.6.1.4.1.8072.9999.9999.1,
 * which will return an integer value when the OID is requested.
 */

#include <nsh.h>

#include "agent.h"

#define oid_netSnmpPlaypen 1, 3, 6, 1, 4, 1, 8072, 9999, 9999
#define oid_exampleOid     oid_netSnmpPlaypen, 1

nsh_scalar_handler_const(exampleOid, ASN_INTEGER, 10);

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
