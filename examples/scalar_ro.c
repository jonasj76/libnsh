/* libnsh example -- Read-only OID
 *
 * This example will register an OID, 1.3.6.1.4.1.8072.9999.9999.1,
 * which will return a random value when the OID is requested.
 */

#include <stdlib.h>
#include <time.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "scalar.h"

#define oid_netSnmpPlaypen 1, 3, 6, 1, 4, 1, 8072, 9999, 9999
#define oid_exampleOid     oid_netSnmpPlaypen, 1

static int callback(void *value, int len, int id)
{
	*(long*)value = rand() % 100;

	return SNMP_ERR_NOERROR;
}

nsh_scalar_handler_ro(exampleOid, ASN_INTEGER, callback, sizeof(long));

int main(void)
{
	srand(time(NULL));

	/* TODO: Code to setup SNMP daemon */

	nsh_register_scalar_ro(exampleOid);

	/* TODO: Event loop for SNMP requests */
}

/**
 * Local Variables:
 *  c-file-style: "linux"
 * End:
 */
