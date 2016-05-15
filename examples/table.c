/* libnsh example - Table
 *
 * Copyright (c) 2016  Jonas Johansson <jonasj76@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <nsh.h>

#include "agent.h"

#define oid_netSnmpPlaypen 1, 3, 6, 1, 4, 1, 8072, 9999, 9999
#define oid_exampleOid     oid_netSnmpPlaypen, 1

#define NUM_TABLE_ENTRIES 2
#define NUM_INDEXES 1

typedef struct table_data_t {
	long idx;
	long data;

	struct table_data_t *next;
} table_data_t;

static struct table_data_t *table_head = NULL;

static nsh_table_index_t idx[NUM_INDEXES] = {
    NSH_TABLE_INDEX (ASN_INTEGER, table_data_t, idx, 0),
};

nsh_table(table_reg, table_get_first, table_get_next, table_free, table_data_t, table_head, NUM_TABLE_ENTRIES, idx, NUM_INDEXES);

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

	return nsh_handle_table(reqinfo, requests, table, NUM_TABLE_ENTRIES);
}

int main(void)
{
	oid table_oid[] = { oid_exampleOid };

	/* Init SNMP subagent */
	agent_init();

	nsh_register_table_ro("exampleOid",
			      table_oid,
			      OID_LENGTH (table_oid),
			      table_handler,
			      &table_reg,
			      table_load);

	/* Event loop to handle SNMP requests */
	agent_event_loop();
}

/**
 * Local Variables:
 *  c-file-style: "linux"
 * End:
 */
