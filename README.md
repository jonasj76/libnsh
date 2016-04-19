# libnsh
[![Build Status](https://travis-ci.org/jonasj76/libnsh.svg?branch=master)](https://travis-ci.org/jonasj76/libnsh)

Net-SNMP helper C-library.

Coding MIBs can be a long and troblesome process. Much of the code for
different OIDs may also look the same except for minor segments that handles
the actual requested OID value. This library will try to simplify the MIB coding
by offer a simple API that hides the duplicated code parts and only focus on the
parts that is needed to get the actual value for a specific OID.

## Build and installation
`libnsh` uses GNU's autotools as its build system.

    ./configure
    make
    make install   # you probably need to be root for this

When checking out the code from GitHub, use `./autogen.sh` to generate the
`configure` script.

## Documentation
This file and the API reference at [http://jonasj76.github.io/libnsh][1].
The API reference is also distributed with the source and can be build using
doxygen.

    cd doc
    doxygen doxygen.conf

## OID

The OID is defined using a `#define` statement. The define **must** have a name
prefixed by `oid_` and the value **must** consist of numbers separated with
commas.

#### Example
    #define oid_myExampleOid 1, 2, 3

## Scalars API

### Register
The register functions will register a read-only or a read-write scalar.
`name` is the name of the defined OID but without the `oid_` prefix.

    nsh_register_scalar_ro(name)
    nsh_register_scalar_rw(name)

### Handlers
All handlers requires a `name` parameter to connect a registered OID to a
handler. `name` is the defined OID but without the `oid_` prefix, i.e. the same
parameter as passed to `nsh_register_scalar_ro()` or `nsh_register_scalar_rw()`.

#### read-only handlers
    nsh_scalar_handler_const(name, type, value)
    nsh_scalar_handler_ro(name, type, get_callback, size)
    nsh_scalar_str_handler_ro(name, get_callback, max_size)
    nsh_scalar_group_handler_ro(name, type, id, get_callback, size, isstring)

#### read-write handlers
    nsh_scalar_handler_rw(name, type, get_callback, size, set_callback)
    nsh_scalar_str_handler_rw(name, get_callback, max_size, set_callback)
    nsh_scalar_group_handler_rw(name, type, id, get_callback, size, isstring, set_callback)

#### get callback
A get callback is defined in the following way:

    int callback(void *value, int len, int id)

The callback function shall set `value` to the return value for the requested
OID. `len` specifies the data size of the requested object, and `id` specifies
the object which is beeing requested, i.e. to distinguish the requested OID from
others OIDs if this callback is shared between several handlers, else this
parameter can be ignored.

#### set callback
A set callback is defined in the following way:

    int callback(void *value, int id)

The callback function shall use `value` to update the corresponding setting in
the applications. `id` specifies the object which is beeing updated, i.e. to
distinguish the requested OID from others OIDs if this callback is shared
between several handlers, else this parameter can be ignored.

## Table API

### Table data struct
A table needs a linked list to store its data. The linked list node shall
contain all data elements for the table and a next node pointer which **must**
be named `next`. A variable for the list shall then be created and initalized
to `NULL`.

#### Example
    typedef struct table_data_t {
        long idx;
        long data;
        struct table_data_t *next;
    }
    struct table_data_t *table_head = NULL;

### Table index struct
An index data struct variable, `nsh_table_index_t`, shall be created and filled
with table index data. The `NSH_TABLE_INDEX` macro can be used to fill the
struct.

    NSH_TABLE_INDEX(type, struct, object, isstring)

The index `type` can be `ASN_COUNTER`, `ASN_INTEGER`, `ASN_TIMETICKS`,
`ASN_UNSIGNED`, `ASN_IPADDRESS`, `ASN_OCTET_STR` or `ASN_OBJECT_ID`. The
*struct* parameter is the name of the table data struct and the *object*
parameter is the the object/element in the struct that hold the index value.
If `isstring` is set and `type` is `ASN_OCTET_STR` the index will be displayed
as a string. If a table have multiple indexes, multiple entries are simply added
to the variable.

#### Example
    nsh_table_index_t index[] = {
        NSH_TABLE_INDEX (ASN_INTEGER, table_data_t, index1, 0),
        NSH_TABLE_INDEX (ASN_INTEGER, table_data_t, index2, 0),
    };

### Table data list functions

    nsh_table_free(name, struct, head)

This function will create a *free* function for the table data list. `name` is
the name of the function that will be created, `struct` is the name of the
table data struct and `head` is the name of the table data list variable.
`name` will be of type `NetsnmpCacheFree`.

    nsh_table_get_next(name, struct, index, num_indexes)

This function will create a *get next entry* function for the table data list.
`name` is the name of the function that will be created, `struct` is the name
of the table data struct, `index` is the name of the index data struct variable
and `num_indexes` is the number of indexes in the `index` list. `name`
will be of type `Netsnmp_Next_Data_Point`.

    nsh_table_get_first(name, get_next, table_head)

This function will create a *get first entry* function for the table data list.
`name` is the name of the function that will be created, `get_next` is the name
of the *get next entry* function and `head` is the name of the table data list
variable. `name` will be of type `Netsnmp_First_Data_Point`.

#### Example
    nsh_table_free(table_free, table_data_t, table_head)
    nsh_table_get_next(table_get_next, table_data_t, index, 2)
    nsh_table_get_first(table_get_first, table_get_next, table_head)

### Register

    nsh_register_table(name, oid, oid_length, min_column, max_column,
                       index_list, num_indexes,  table_handler, table_get_first,
                       table_get_next, table_load, table_free, access)

`name` is a string to identify the the handler.`oid` is the OID to be
registered. `oid_length` is the length of the `oid`. The `OID_LENGTH(oid)`
shall be used to set this value. `min_column` is the starting column in the
table, usually 1. `max_column` is the last column in the table to be handled.
`index_list` is an integer array which holds the table indexes types.
`num_indexes` is the numer of indexes in `index_list`. `table_handler` is the
name of the function for the table handler, see below. `table_get_first` is the
name of the *get first entry* in table data list function. `table_load` is the
name of the table load function, see below. `table_free` is the name of the
*free* table entry in table data list function. *access* shall be set to
`HANDLER_CAN_RONLY`.

#### Example
    oid table_oid[] = { oid_exampleOid };
    int index[]     = { ASN_INTEGER };

    nsh_register_table("exampleOid", table_oid, OID_LENGTH (table_oid),
                       1, 2 index, 1, table_handler,
                       table_get_first, table_get_next, table_load, table_free,
                       HANDLER_CAN_RONLY);

### Table handler

A table handler function is needed to handle table requests.

    int table_handler(netsnmp_mib_handler *handler,
                      netsnmp_handler_registration *reginfo,
                      netsnmp_agent_request_info *reqinfo,
                      netsnmp_request_info *requests)

In this function a table entry struct variable, `nsh_table_entry_t`, shall be
created and filled with table entry data. The `NSH_TABLE_ENTRY_RO` macro can be
used to fill the struct.

    NSH_TABLE_ENTRY_RO(type, struct, object, isstring)

The `type` can be `ASN_COUNTER`, `ASN_INTEGER`, `ASN_TIMETICKS`,
`ASN_UNSIGNED`, `ASN_IPADDRESS`, `ASN_OCTET_STR` or `ASN_OBJECT_ID`. The
*struct* parameter is the name of the table data struct and the *object*
parameter is the the object/element in the struct that hold the data value.
If `isstring` is set and `type` is `ASN_OCTET_STR` the data will be displayed
as a string. A entry for each column is added to the variable.

#### Example
    nsh_table_entry_t table[] = {
        NSH_TABLE_ENTRY_RO (ASN_INTEGER, table_data_t, idx,  0),
        NSH_TABLE_ENTRY_RO (ASN_INTEGER, table_data_t, data, 0),
    };


The `nsh_handle_table()` function shall then be called to handle the table.

    nsh_handle_table(reqinfo, requests, table, table_size)

`reqinfo` and `requests` are parameters passed from the table handler function.
`table` is the table struct variable and `table_size` is the number of elements
in `table`.

### Table load callback

A table load function is needed to fill the table with data.

    int table_load(netsnmp_cache *cache, void* vmagic)

In this function a new table entry shall be allocated for each row. An entry is
allocated with `SNMP_MALLOC_TYPEDEF()`. The allocated entry is then filled with
data and the table data list is updated.

#### Example
    table_data_t *entry;

    entry = SNMP_MALLOC_TYPEDEF (table_data_t);
    if (!entry)
        return;

    entry->idx  = idx;
    entry->data = data;
    entry->next = table_head;
    table_head  = entry;

[1]: http://jonasj76.github.io/libnsh
