/* Scalar helpers
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

#include "nsh.h"
#include "private.h"

static int __mode_get(u_char type,
		      int id,
		      nsh_get_cb cb,
		      int len,
		      long get_arg,
		      netsnmp_agent_request_info *reqinfo,
		      netsnmp_request_info *requests)
{
	void *val = NULL;
	char* buf = NULL;
	int ret   = SNMP_ERR_GENERR;

	switch (type) {
	case ASN_COUNTER:
	case ASN_INTEGER:
	case ASN_TIMETICKS:
	case ASN_UNSIGNED:
	case ASN_IPADDRESS:
		/* handle constant value OID */
		if (!cb) {
			val = &get_arg;
			ret = SNMP_ERR_NOERROR;
			break;
		}
	case ASN_OBJECT_ID:
	case ASN_OCTET_STR:
		if (!cb)
			break;
		/* adjust length for OID-type OID */
		if (type == ASN_OBJECT_ID)
			len *= sizeof(oid);
		val = buf = malloc(len);
		ret = cb(val, len, id);
		if (get_arg && type == ASN_OCTET_STR)
			len = strlen(val);
		break;
	}

	if (ret != SNMP_ERR_NOERROR)
		netsnmp_set_request_error(reqinfo, requests, ret);
	else
		snmp_set_var_typed_value(requests->requestvb, type, val, len);

	if (buf)
		free(val);

	return ret;
}

static int __set_reserve1(u_char type,
			  size_t size,
			  nsh_set_cb cb,
			  netsnmp_agent_request_info *reqinfo,
			  netsnmp_request_info *requests)
{
	int ret;

	if (!cb) {
		netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_NOTWRITABLE);
		return SNMP_ERR_NOTWRITABLE;
	}

	ret = netsnmp_check_vb_type_and_max_size(requests->requestvb, type, size);
	if (ret != SNMP_ERR_NOERROR)
		netsnmp_set_request_error(reqinfo, requests, ret);

	return ret;
}

static int __set_action(u_char type,
			size_t len,
			int id,
			nsh_set_cb cb,
			netsnmp_agent_request_info *reqinfo,
			netsnmp_request_info *requests)
{
	int ret = SNMP_ERR_GENERR;

	if (!cb) {
		netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_NOTWRITABLE);
		return SNMP_ERR_NOTWRITABLE;
	}

	switch (type) {
	case ASN_COUNTER:
	case ASN_INTEGER:
	case ASN_TIMETICKS:
	case ASN_UNSIGNED:
	case ASN_IPADDRESS:
		ret = cb(requests->requestvb->val.integer, id);
		break;

	case ASN_OCTET_STR:
		ret = cb(requests->requestvb->val.string, id);
		break;

	case ASN_OBJECT_ID:
		ret = cb(requests->requestvb->val.objid, id);
		break;
	}

	if (ret != SNMP_ERR_NOERROR)
		netsnmp_request_set_error(requests, ret);

	return ret;
}

/**
 * __nsh_scalar_handler - Scalar handler
 * @type     : OID type:
 *             %ASN_COUNTER, %ASN_INTEGER, %ASN_TIMETICKS, %ASN_UNSIGNED,
 *             %ASN_IPADDRESS, %ASN_OCTET_STR or %ASN_OBJECT_ID
 * @id:      : ID in a scalar group, ignored if grouping is not used.
 *             A group of scalar will share the same @get_cb function and @id
 *             is used to distinguish the OID requests from each other.
 * @get_cb   : Callback function to get OID value.
 *             If %NULL and @type is not set to %ASN_OCTET_STR or
 *             %ASN_OBJECT_ID, the value of @get_arg will be returned.
 * @get_sz   : Length of returned value.
 *             When @type is %ASN_OBJECT_ID this will be the size of OID.
 *             When @type is %ASN_OCTET_STR this will be the size of the array
 *             or the maximum string size if @get_arg is set.
 * @get_arg  : Used as return value if %get_cb is %NULL.
 *             If set and @type is %ASN_OCTET_STR, the value returned is
 *             treated as a string and will be of length strlen() instead of
 *             @get_sz.
 * @handler  : Net-Snmp parameter
 * @reginfo  : Net-Snmp parameter, Not used
 * @reqinfo  : Net-Snmp parameter, Not used
 * @requests : Net-Snmp parameter
 *
 * Returns:
 * %SNMP_ERR_NOERROR on success.
 */
int __nsh_scalar_handler(u_char type,
			 int id,
			 nsh_get_cb get_cb,
			 int get_sz,
			 long get_arg,
			 nsh_set_cb set_cb,
			 netsnmp_mib_handler *UNUSED(handler),
			 netsnmp_handler_registration *UNUSED(reginfo),
			 netsnmp_agent_request_info *reqinfo,
			 netsnmp_request_info *requests)
{
	switch (reqinfo->mode) {
	case MODE_GET:
		return __mode_get(type, id, get_cb, get_sz, get_arg, reqinfo, requests);

	case MODE_SET_RESERVE1:
		return __set_reserve1(type, get_sz, set_cb, reqinfo, requests);

	case MODE_SET_RESERVE2:
	case MODE_SET_FREE:
		return SNMP_ERR_NOERROR;

	case MODE_SET_ACTION:
		return __set_action(type, get_sz, id, set_cb, reqinfo, requests);

	case MODE_SET_UNDO:
	case MODE_SET_COMMIT:
		return SNMP_ERR_NOERROR;
	}

	return SNMP_ERR_GENERR;
}

/**
 * Local Variables:
 *  c-file-style: "linux"
 * End:
 */
