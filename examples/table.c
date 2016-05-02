/* libnsh example -- Table
 *
 * This example will register an table on OID 1.3.6.1.e4.1.8072.9999.9999.1.
 * The table will have two(2) entries (idx, data) and will consist of
 * three(3) rows.
 */

#include "nsh.h"

#define oid_netSnmpPlaypen 1, 3, 6, 1, 4, 1, 8072, 9999, 9999
#define oid_exampleOid     oid_netSnmpPlaypen, 1

#define MIN_COLUMN 1
#define MAX_COLUMN 2

#define NUM_INDEXES 1

typedef struct table_data_t {
	long idx;
	long data;

	struct table_data_t *next;
} table_data_t;

static struct table_data_t *table_head = NULL;

static nsh_table_index_t idx[] = {
    NSH_TABLE_INDEX (ASN_INTEGER, table_data_t, idx, 0),
};

nsh_table(table_get_first, table_get_next, table_free, table_data_t, table_head, idx, NUM_INDEXES);

static void table_create_entry(long idx,
			       long data)
{
	table_data_t *entry;

	entry = SNMP_MALLOC_TYPEDEF (table_data_t);
	if (!entry)
		return;

	entry->idx  = idx;
	entry->data = data;

	entry->next = table_head;
	table_head  = entry;
}

static int table_load (netsnmp_cache *cache, void* vmagic)
{
	const int num_rows = 3;
	int row;

	for (row = 0; row < num_rows; row++) {
		table_create_entry(row, row*row);
	}

	return 0;
}

static int table_handler(netsnmp_mib_handler *handler,
			 netsnmp_handler_registration *reginfo,
			 netsnmp_agent_request_info *reqinfo,
			 netsnmp_request_info *requests)
{
	nsh_table_entry_t table[] = {
		NSH_TABLE_ENTRY_RO (ASN_INTEGER, table_data_t, idx,  0),
		NSH_TABLE_ENTRY_RO (ASN_INTEGER, table_data_t, data, 0),
	};

	return nsh_handle_table(reqinfo, requests, table, MAX_COLUMN);
}

int main(void)
{
	oid table_oid[] = { oid_exampleOid };
	int index[]     = { ASN_INTEGER };

	/* TODO: Code to setup SNMP daemon */

	nsh_register_table("exampleOid",
			   table_oid,
			   OID_LENGTH (table_oid),
			   MIN_COLUMN,
			   MAX_COLUMN,
			   index,
			   NUM_INDEXES,
			   table_handler,
			   table_get_first,
			   table_get_next,
			   table_load,
			   table_free,
			   HANDLER_CAN_RONLY);

	/* TODO: Event loop for SNMP requests */
}

/**
 * Local Variables:
 *  c-file-style: "linux"
 * End:
 */
