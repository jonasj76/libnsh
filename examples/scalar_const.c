/* libnsh example -- Constant OID
 *
 * This example will register an OID, 1.3.6.1.4.1.8072.9999.9999.1,
 * which will return the value 10 when the OID is requested.
 */

#include "nsh.h"

#define oid_netSnmpPlaypen 1, 3, 6, 1, 4, 1, 8072, 9999, 9999
#define oid_exampleOid     oid_netSnmpPlaypen, 1

nsh_scalar_handler_const(exampleOid, ASN_INTEGER, 10);

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
