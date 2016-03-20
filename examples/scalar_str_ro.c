/* libnsh example -- String OID
 *
 * This example will register an OID, 1.3.6.1.4.1.8072.9999.9999.1,
 * which will return a string when the OID is requested.
 */

#include <stdio.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "../scalar.h"

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
	/* TODO: Code to setup SNMP daemon */

	nsh_register_scalar_ro(exampleOid);

	/* TODO: Event loop for SNMP requests */
}

/**
 * Local Variables:
 *  c-file-style: "linux"
 * End:
 */
