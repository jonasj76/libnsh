# libnsh
Net-SNMP helper C-library.

Coding MIBs can be a long and troblesome process. Much of the code for
different OIDs may also look the same except for minor segments that handles
the actual requested OID value. This library will try to simplify the MIB coding
by offer a simple API that hides the duplicated code parts and only focus on the
parts that is needed to get the actual value for a specific OID.

## OID

The OID is defined using a `#define` statement. The define **must** have a name
prefixed by `oid_` and the value **must** consist of numbers separated with
commas.

#### Example
    #define oid_myExampleOid 1, 2, 3

## Scalars API

### Register
    nsh_register_scalar_ro(name)

This function will register a read-only scalar. `name` is the name of the
defined OID but without the `oid_` prefix.

#### Example
    nsh_register_scalar_ro(myExampleOid)

### Handlers
All handlers requires a `name` parameter to connect a registered OID to a
handler. `name` is the defined OID but without the `oid_` prefix, i.e. the
same parameter as passed to `nsh_register_scalar_ro()`.

#### Constant handler
    nsh_scalar_handler_const(name, type, value)

This function will create a handler for the registered `name` OID. A SNMP
request to the OID will return a `type` of `ASN_COUNTER`, `ASN_INTEGER`,
`ASN_TIMETICKS`, `ASN_UNSIGNED` or `ASN_IPADDRESS` with the value `value`.

#### Read-only handler
    nsh_scalar_handler_ro(name, type, callback, size)

This function will create a read-only handler for the registered `name` OID. A
SNMP request will  return the value from the `callback` function. The `type` can
be `ASN_COUNTER`, `ASN_INTEGER`, `ASN_TIMETICKS`, `ASN_UNSIGNED`,
`ASN_IPADDRESS`, `ASN_OCTET_STR` or `ASN_OBJECT_ID`. `size` is the data size of
the returned value.

#### String handler
    nsh_scalar_str_handler_ro(name, callback, max_size)

This function behaves in the same maner as `nsh_scalar_handler_ro()` except that
the type is set to `ASN_OCTET_STR` and the length of the value returned from
`callback` is set to the string length of the returned string. The `max_size`
parameter states the maximum string length that can be returned.

#### Read-only group handler
    nsh_scalar_group_handler_ro(name, type, id, callback, size, isstring)

This function also behave in the same maner as `nsh_scalar_handler_ro()` but an
`id` parameter is also passed. This parameter is used to distinguish SNMP
requests for different OIDs if the `callback` is shared between several
handlers. If `isstring` is set and `type` is `ASN_OCTET_STR` the length of the
requested value is set to the string length of the value, else it will be set to
`size`.

### Callbacks
A callback is defined in the following way:

    int callback(void *value, int len, int id)

The callback function shall set `value` to the return value for the requested
OID. `len` specifies the data size of the requested object, and `id` specifies
the object which is beeing requested, i.e. to distinguish the requested OID from
others OIDs if this callback is shared between several handlers, else this
parameter can be ignored.
