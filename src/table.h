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

#ifndef NSH_TABLE_H_
#define NSH_TABLE_H_

#include <stddef.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "private.h"

/**
 * nsh_table_index_t - Table index struct
 * @type : Index type:
 *         %ASN_COUNTER, %ASN_INTEGER, %ASN_TIMETICKS, %ASN_UNSIGNED,
 *         %ASN_IPADDRESS, %ASN_OCTET_STR or %ASN_OBJECT_ID
 * @len  : Length of index object.
 *         If @len = -1 and @type is %ASN_OCTET_STR, the length
 *         will be set by using strlen().
 * @ofs  : Offset in table data struct where object can be found.
 */
typedef struct nsh_table_index_t {
	int type;
	int len;
	int ofs;
	int isstring;
} nsh_table_index_t;

/**
 * NSH_TABLE_INDEX - Macro to fill a @nsh_table_index_t entry
 * @_type       : Index type, see @nsh_table_index_t.
 * @_table_data : Table data struct of type @nsh_table_index_t.
 * @_obj        : Element in @_table_data.
 * @_isstring   : Element is a string.
 */
#define NSH_TABLE_INDEX(_type, _struct, _obj, _isstring) \
	{.type = _type, .len = ENTRY_SIZE(_struct, _obj), .ofs = offsetof (struct _struct, _obj), .isstring = _isstring}

/**
 * nsh_table_entry_t - Table entry struct
 * @type : Object type:
 *         %ASN_COUNTER, %ASN_INTEGER, %ASN_TIMETICKS, %ASN_UNSIGNED,
 *         %ASN_IPADDRESS, %ASN_OCTET_STR or %ASN_OBJECT_ID
 * @len  : Length of object.
 * @ofs  : Offset in table data struct where object can be found.
 */
typedef struct nsh_table_entry_t {
	int type;
	int len;
	int ofs;
	int isstring;
	int (*set_cb)(long idx, void *value);
} nsh_table_entry_t;

/**
 * NSH_TABLE_ENTRY - Macro to fill a @nsh_table_entry_t entry
 * @_type       : Index type, see @nsh_table_entry_t.
 * @_table_data : Table data struct of type @nsh_table_entry_t.
 * @_obj        : Element in @_table_data.
 * @_isstring   : Element is a string.
 */
#define NSH_TABLE_ENTRY(_type, _struct, _obj,  _isstring, _set_cb) \
	{.type = _type, .len = ENTRY_SIZE(_struct, _obj), .ofs = offsetof(struct _struct, _obj), .isstring = _isstring, .set_cb = _set_cb}
#define NSH_TABLE_ENTRY_RO(_type, _struct, _obj, _isstring) \
	NSH_TABLE_ENTRY(_type, _struct, _obj, _isstring, NULL)
#define NSH_TABLE_ENTRY_RW(_type, _struct, _obj, _isstring, _set_cb) \
	NSH_TABLE_ENTRY(_type, _struct, _obj, _isstring, _set_cb)

/**
 * nsh_register_table_ro - Register a read-only table handler
 * @name            : OID name of table. A string.
 * @table_oid       : OID to register.
 * @oid_len         : Length of OID. Use OID_LENGTH(@table_oid).
 * @table_handler   : Name of callback to table handler function.
 * @table_reg       : Table register struct of type @nsh_table_reg_t.
 * @table_load_hook : Name of callback to table load hook function.
 *
 * This function will register a tabler handler callback for the OID @table_oid.
 * The callbacks for @table_reg struct is created by the nsh_table() macro.
 *
 * Returns:
 * %MIB_REGISTERED_OK is returned on success.
 * Failures are %MIB_REGISTRATION_FAILED and %MIB_DUPLICATE_REGISTRATION.
 */
int nsh_register_table_ro(const char*              name,
			  const oid                *table_oid,
			  size_t                   oid_len,
			  Netsnmp_Node_Handler     *table_handler,
			  nsh_table_reg_t          *table_reg,
			  NetsnmpCacheLoad         *table_load_hook);

/**
 * nsh_register_table_rw - Register a writable table handler
 * @name            : OID name of table. A string.
 * @table_oid       : OID to register.
 * @oid_len         : Length of OID. Use OID_LENGTH(@table_oid).
 * @table_handler   : Name of callback to table handler function.
 * @table_reg       : Table register struct of type @nsh_table_reg_t.
 * @table_load_hook : Name of callback to table load hook function.
 *
 * This function will register a tabler handler callback for the OID @table_oid.
 * The callbacks for @table_reg struct is created by the nsh_table() macro.
 *
 * Returns:
 * %MIB_REGISTERED_OK is returned on success.
 * Failures are %MIB_REGISTRATION_FAILED and %MIB_DUPLICATE_REGISTRATION.
 */
int nsh_register_table_rw(const char*              name,
			  const oid                *table_oid,
			  size_t                   oid_len,
			  Netsnmp_Node_Handler     *table_handler,
			  nsh_table_reg_t          *table_reg,
			  NetsnmpCacheLoad         *table_load_hook);

/**
 * nsh_table_free - Empty table elements
 * @_name       : Name of the function to be created.
 * @_table_data : Table data struct.
 * @_head       : Head of table data.
 *
 * This will run before an eventual table load to make sure that
 * everything is empty.
 */
#define nsh_table_free(_name, _table_data, _head)				\
static void _name(netsnmp_cache* UNUSED(cache), 				\
                  void* UNUSED(vmagic)) 					\
{										\
	_table_data *this, *that;						\
										\
	for (this = _head; this; this = that) {					\
		that = this->next;						\
		SNMP_FREE (this);						\
	}									\
	_head = NULL;								\
}

/**
 * nsh_table_get_first - Get first table element
 * @_name          : Name of function to be created.
 * @table_get_next : Name of callback to table get next function.
 * @_head          : Head of table data.
 */
#define nsh_table_get_first(_name, _table_get_next, _head)			\
static netsnmp_variable_list* _name(void **loop_context,			\
				    void **data_context,			\
				    netsnmp_variable_list *put_index_data,	\
				    netsnmp_iterator_info *data)		\
{										\
	*loop_context = _head;							\
	return _table_get_next(loop_context, data_context,			\
			       put_index_data, data);				\
}

/**
 * nsh_table_get_next - Get next table element
 * @_name       : Name of function to be created.
 * @_table_data : Table data struct.
 * @_idx_list   : Pointer to index list.
 * @_num_idx    : Number of indexes in table index list.
 */
#define nsh_table_get_next(_name, _table_data, _idx_list, _num_idx)		\
typedef nsh_table_index_t* (*nsh_table_index_cb)(void); 			\
										\
static nsh_table_index_t* FN_INDEX(_name)(void) 				\
{										\
	return _idx_list;							\
}										\
										\
static netsnmp_variable_list* _name (void **loop_context,			\
				     void **data_context,			\
				     netsnmp_variable_list *put_index_data,	\
				     netsnmp_iterator_info* UNUSED(data))	\
{										\
	_table_data *entry = (_table_data*)*loop_context;			\
	netsnmp_variable_list *idx = put_index_data;				\
	nsh_table_index_cb idx_list_cb = FN_INDEX(_name);	       		\
	nsh_table_index_t *index = idx_list_cb();			       	\
	int i, len;								\
										\
	if (!entry)								\
		return NULL;							\
										\
	for (i = 0; i < _num_idx; i++) {					\
		len = index[i].len;						\
		if (len < 0 && (index[i].type == ASN_OCTET_STR))		\
			len = strlen(((char*)entry + index[i].ofs));		\
		snmp_set_var_typed_value (idx, index[i].type,			\
					  (char*)entry + index[i].ofs, len);	\
		idx = idx->next_variable;					\
	}									\
	*data_context = (void*) entry;						\
	*loop_context = (void*) entry->next;					\
										\
	return put_index_data;							\
}

/**
 * nsh_handle_table - Handle table request
 * @reqinfo  : Net-Snmp parameter.
 * @requests : Net-Snmp parameter.
 * @table    : Pointer to @nsh_table_entry_t list.
 * @table_sz : Number of elements in @table.
 */
int nsh_handle_table(netsnmp_agent_request_info *reqinfo,
		     netsnmp_request_info       *requests,
		     nsh_table_entry_t          *table,
		     int                        table_sz);

#endif /* NSH_TABLE_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 * End:
 */
