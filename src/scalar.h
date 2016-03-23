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

#ifndef NSH_SCALAR_H_
#define NSH_SCALAR_H_

#include "private.h"

/**
 * nsh_register_scalar - Register a OID scalar handler
 * @_name   : OID define. The defined must be named oid_@name and on
 *            format 'x,y,z...'.
 * @_access : HANDLER_CAN_RONLY | HANDLER_CAN_RWRITE
 *
 * This function will register a scalar handler callback for the OID @_oid.
 * The name of the callback function is based on the variable name @_oid,
 * i.e. the same name shall also be used as input to nsh_scalar_handler().
 */
#define nsh_register_scalar(_name, _access)				\
{									\
	oid _o[] = { oid_ ## _name };					\
	netsnmp_handler_registration *_reginfo;				\
									\
	_reginfo = netsnmp_create_handler_registration(			\
		#_name, FN_HANDLE(_name), _o, OID_LENGTH(_o), _access);	\
	if (_reginfo)							\
		netsnmp_register_scalar(_reginfo);			\
}

#define nsh_register_scalar_ro(_name) nsh_register_scalar(_name, HANDLER_CAN_RONLY)

/**
 * nsh_scalar_handler - Create a scalar handler
 * @_oid      : OID define, see nsh_register_scalar().
 * @_type     : OID type:
 *              %ASN_COUNTER, %ASN_INTEGER, %ASN_TIMETICKS, %ASN_UNSIGNED,
 *              %ASN_IPADDRESS, %ASN_OCTET_STR or %ASN_OBJECT_ID
 * @_id:      : ID in a scalar group, ignored if grouping is not used.
 *              A group of scalar will share the same @_get_cb function and @_id
 *              is used to distinguish the OID requests from each other.
 * @_get_cb   : Callback function to get OID value.
 *              If %NULL and @_type is not set to %ASN_OCTET_STR or
 *              %ASN_OBJECT_ID, the value of @_get_arg will be returned.
 * @_get_sz   : Length of returned value.
 *              When @_type is %ASN_OBJECT_ID this will be the size of OID.
 *              When @_type is %ASN_OCTET_STR this will be the size of the array
 *              or the maximum string size if @_get_arg is set.
 * @_get_arg  : Used as return value if %_get_cb is %NULL.
 *              If set and @_type is %ASN_OCTET_STR, the value returned is
 *              treated as a string and will be of length strlen() instead of
 *              @_get_sz.
 *
 * This function will create a scalar handler callback. The callback function
 * name will be based on the name of the @_oid parameter, so the same @_oid name
 * shall be used here as when registering the OID with nsh_register_scalar().
 *
 * Returns:
 * %SNMP_ERR_NOERROR on success.
 */
#define nsh_scalar_handler(_oid, _type, _id,				\
			   _get_cb,  _get_sz, _get_arg)			\
static int FN_HANDLE(_oid)(netsnmp_mib_handler *handler,	       	\
			 netsnmp_handler_registration *reginfo,		\
			 netsnmp_agent_request_info *reqinfo,		\
			 netsnmp_request_info *requests)		\
{									\
	return __nsh_scalar_handler(_type, _id,				\
				    _get_cb, _get_sz, _get_arg,		\
				    handler,				\
				    reginfo, reqinfo, requests);	\
}

#define nsh_scalar_handler_const(_oid, _type, _val) \
        nsh_scalar_handler(_oid, _type, 0, NULL, sizeof(long), _val)

#define nsh_scalar_handler_ro(_oid, _type, _get_cb, _get_sz) \
        nsh_scalar_handler(_oid, _type, 0, _get_cb, _get_sz, 0)

#define nsh_scalar_str_handler_ro(_oid, _get_cb, _max_sz) \
        nsh_scalar_handler(_oid, ASN_OCTET_STR, 0, _get_cb, _max_sz, 1)

#define nsh_scalar_group_handler_ro(_oid, _type, _id, _get_cb, _get_sz, _get_arg) \
        nsh_scalar_handler(_oid, _type, _id, _get_cb, _get_sz, _get_arg)

#endif /* NSH_SCALAR_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 * End:
 */
