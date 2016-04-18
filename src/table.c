/* Table helpers
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

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "private.h"
#include "table.h"

#define TIMEOUT 5

void nsh_register_table(const char*              name,
			const oid                *table_oid,
			size_t                   oid_len,
			unsigned int             min_column,
			unsigned int             max_column,
			int*                     idx_list,
			int                      num_idx,
			Netsnmp_Node_Handler     *table_handler,
			Netsnmp_First_Data_Point *table_get_first,
			Netsnmp_First_Data_Point *table_get_next,
			NetsnmpCacheLoad         *table_load_hook,
			NetsnmpCacheFree         *table_free_hook,
			int                      access)
{
	netsnmp_handler_registration    *reg;
	netsnmp_iterator_info           *iinfo;
	netsnmp_table_registration_info *table_info;
	int i;

	reg = netsnmp_create_handler_registration(name,
						  table_handler,
						  table_oid,
						  oid_len,
						  access);

	table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);
	for (i = 0; i < num_idx; i++)
		netsnmp_table_helper_add_index(table_info, idx_list[i]);
	table_info->min_column = min_column;
	table_info->max_column = max_column;

	iinfo = SNMP_MALLOC_TYPEDEF(netsnmp_iterator_info);
	iinfo->get_first_data_point = table_get_first;
	iinfo->get_next_data_point  = table_get_next;
	iinfo->table_reginfo        = table_info;

	netsnmp_register_table_iterator(reg, iinfo);
	netsnmp_inject_handler_before(reg,
				      netsnmp_get_cache_handler(TIMEOUT,
								table_load_hook,
								table_free_hook,
								table_oid,
								oid_len),
				      TABLE_ITERATOR_NAME);
}

static int __mode_get_req(netsnmp_agent_request_info *reqinfo,
			  netsnmp_request_info       *request,
			  nsh_table_entry_t          *entry)
{
	long *table_entry = netsnmp_extract_iterator_context(request);
	uint8_t *ofs;
	int len, ret;

	if (!table_entry || !entry || !entry->len)
		return netsnmp_request_set_error(request, SNMP_NOSUCHINSTANCE);

	ofs = (uint8_t*)(table_entry) + entry->ofs;
	if (entry->isstring) {
		len = strlen((const char*)ofs);
		if (len > entry->len)
			len = entry->len;
	}
	else {
		len = entry->len;
	}

	/* set data to be returned by a snmp request */
	ret = snmp_set_var_typed_value(request->requestvb, entry->type, ofs, len);
	if (ret) {
		return netsnmp_request_set_error(request, SNMP_ERR_GENERR);
	}

	return SNMP_ERR_NOERROR;
}

static void __mode_get(netsnmp_agent_request_info *reqinfo,
		       netsnmp_request_info       *requests,
		       nsh_table_entry_t          *table,
		       unsigned int               table_sz)
{
	netsnmp_request_info *request;
	netsnmp_table_request_info *table_info;

	for (request = requests; request; request = request->next) {
		table_info = netsnmp_extract_table_info(request);

		if (table_info->colnum <= table_sz)
			__mode_get_req(reqinfo, request, &table[table_info->colnum-1]);
		else
			netsnmp_request_set_error(request, SNMP_NOSUCHOBJECT);
	}
}

static int __set_reserve1_req(netsnmp_agent_request_info *reqinfo,
			      netsnmp_request_info       *request,
			      nsh_table_entry_t          *entry)
{
	long *table_entry = netsnmp_extract_iterator_context(request);
	int ret;

	if (!entry || !entry->set_cb)
		return netsnmp_request_set_error(request, SNMP_ERR_NOTWRITABLE);

	/* check type and size */
	ret = netsnmp_check_vb_type_and_max_size(request->requestvb, entry->type, entry->len);
	if (ret != SNMP_ERR_NOERROR) {
		return netsnmp_request_set_error(request, ret);
	}

	return ret;
}

static void __set_reserve1(netsnmp_agent_request_info *reqinfo,
			   netsnmp_request_info       *requests,
			   nsh_table_entry_t          *table,
			   unsigned int               table_sz)
{
	netsnmp_request_info *request;
	netsnmp_table_request_info *table_info;

	for (request = requests; request; request = request->next) {
		table_info = netsnmp_extract_table_info(request);

		if (table_info->colnum <= table_sz)
			__set_reserve1_req(reqinfo, request, &table[table_info->colnum-1]);
		else
			netsnmp_request_set_error(request, SNMP_ERR_NOTWRITABLE);
	}
}

static int __action_req(netsnmp_agent_request_info *reqinfo,
			netsnmp_request_info       *request,
			nsh_table_entry_t          *entry)
{
	netsnmp_table_request_info *table_info = netsnmp_extract_table_info(request);
	int ret;

	if (!entry || !entry->set_cb)
		return netsnmp_request_set_error(request, SNMP_ERR_GENERR);

	switch (entry->type) {
	case ASN_INTEGER:
	case ASN_UNSIGNED:
	case ASN_IPADDRESS:
		ret = entry->set_cb(*table_info->indexes->val.integer, request->requestvb->val.integer);
		break;
	case ASN_OCTET_STR:
		ret = entry->set_cb(*table_info->indexes->val.integer, request->requestvb->val.string);
		break;
	case ASN_OBJECT_ID:
		ret = entry->set_cb(*table_info->indexes->val.integer, request->requestvb->val.objid);
		break;
	default:
		ret = SNMP_ERR_GENERR;
	}

	return netsnmp_request_set_error(request, ret);
}

static void __set_action(netsnmp_agent_request_info *reqinfo,
			 netsnmp_request_info       *requests,
			 nsh_table_entry_t          *table,
			 unsigned int               table_sz)
{
	netsnmp_request_info *request;
	netsnmp_table_request_info *table_info;
	int ret;

	for (request = requests; request; request = request->next) {
		table_info = netsnmp_extract_table_info(request);

		if (table_info->colnum <= table_sz)
			__action_req(reqinfo, request, &table[table_info->colnum-1]);
		else
			netsnmp_request_set_error(request, SNMP_ERR_NOTWRITABLE);
	}
}

int nsh_handle_table(netsnmp_agent_request_info *reqinfo,
		     netsnmp_request_info       *requests,
		     nsh_table_entry_t          *table,
		     int                        table_sz)
{
	switch (reqinfo->mode) {
	case MODE_GET:
		__mode_get(reqinfo, requests, table, table_sz);
		break;

	case MODE_SET_RESERVE1:
		__set_reserve1(reqinfo, requests, table, table_sz);
		break;

	case MODE_SET_RESERVE2:
	case MODE_SET_FREE:
		break;

	case MODE_SET_ACTION:
		__set_action(reqinfo, requests, table, table_sz);
		break;

	case MODE_SET_UNDO:
	case MODE_SET_COMMIT:
		break;
	}

	return SNMP_ERR_NOERROR;
}

/**
 * Local Variables:
 *  c-file-style: "linux"
 * End:
 */
