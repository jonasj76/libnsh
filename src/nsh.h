/* libnsh - Net-SNMP helper C-library
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

#ifndef NSH_H_
#define NSH_H_

#include "table.h"
#include "private.h"

/**
 * Register a read-only scalar OID.
 * @param name  OID define, without the \a oid_ prefix.
 *
 * This function will register a read-only scalar handler callback for the
 * defined OID. The name of the callback function is based on the input
 * parameter \a name, which means that \a name shall also be the input to the
 * scalar create function which will handle the OID.
 */
#define nsh_register_scalar_ro(name) \
	_nsh_register_scalar(name, HANDLER_CAN_RONLY)

/**
 * Register a writable scalar OID.
 * @param name  OID define without the \a oid_ prefix.
 *
 * This function will register a read-write scalar handler callback for the
 * defined OID. The name of the callback function is based on the input
 * parameter \a name, which means that \a name shall also be the input to the
 * scalar create function which will handle the OID.
 */
#define nsh_register_scalar_rw(name) \
	_nsh_register_scalar(name, HANDLER_CAN_RWRITE)

/**
 * Create a constant scalar OID handler.
 * @param name   OID define, without the \a oid_ prefix.
 * @param type   OID type.
 *               %ASN_COUNTER, %ASN_INTEGER, %ASN_TIMETICKS, %ASN_UNSIGNED or
 *               %ASN_IPADDRESS.
 * @param value  Value to be returned by a request.
 *
 * This function will create a scalar handler callback function which will
 * return a constant value.
 * The callback function name will be based on the input parameter \a name,
 * which should be the same parameter used when registering the OID, i.e with
 * nsh_register_scalar_ro().
 */
#define nsh_scalar_handler_const(name, type, value) \
        _nsh_scalar_handler(name, type, 0, NULL, sizeof(long), value, NULL)

/**
 * Create a read-only scalar OID handler.
 * @param name      OID define, without the \a oid_ prefix.
 * @param type      OID type.
 *                  %ASN_COUNTER, %ASN_INTEGER, %ASN_TIMETICKS, %ASN_UNSIGNED,
 *                  %ASN_IPADDRESS, %ASN_OCTET_STR or %ASN_OBJECT_ID.
 * @param callback  Callback function to get return value by a request.
 * @param size      Data length of handled value.
 *
 * This function will create a read-only scalar handler callback function which
 * will return the value from the \a callback function. If a string value is to
 * be returned, consider using nsh_scalar_str_handler_ro() instead, else the
 * string returned will always be \a size long.
 * The callback function name will be based on the input parameter \a name,
 * which should be the same parameter used when registering the OID, i.e with
 * nsh_register_scalar_ro().
 */
#define nsh_scalar_handler_ro(name, type, callback, size) \
	_nsh_scalar_handler(name, type, 0, callback, size, 0, NULL)

/**
 * Create a read-only string OID handler.
 * @param name        OID define, without the \a oid_ prefix.
 * @param callback    Callback function to get string value by a request.
 * @param max_length  Maximum length of returned string.
 *
 * This function will create a read-only string handler callback function which
 * will return the string from the \a callback function. The function is similar
 * to nsh_scalar_handler_ro() except it will return a string with the correct
 * length, i.e. the actual string length instead of the maximum length of the
 * string.
 * The callback function name will be based on the input parameter \a name,
 * which should be the same parameter used when registering the OID, i.e with
 * nsh_register_scalar_ro().
 */
#define nsh_scalar_str_handler_ro(name, callback, max_length) \
	_nsh_scalar_handler(name, ASN_OCTET_STR, 0, callback, max_length, 1, NULL)

/**
 * Create a grouped read-only scalar OID handler.
 * @param name      OID define, without the \a oid_ prefix.
 * @param type      OID type.
 *                  %ASN_COUNTER, %ASN_INTEGER, %ASN_TIMETICKS, %ASN_UNSIGNED,
 *                  %ASN_IPADDRESS, %ASN_OCTET_STR or %ASN_OBJECT_ID.
 * @param id        Group identifier for this request.
 * @param callback  Callback function to get return value by a request.
 * @param size      Data length of handled value.
 * @param isstring  Set if value is a string.
 *
 * This function will create a grouped read-only scalar handler callback
 * function. This function will behave in the same maner as
 * nsh_scalar_handler_ro() except that a \a id is used to distinguish different
 * OID requests. A common \a callback can therefore be shared between several
 * created handlers. If the \a isstring parameter is set, the function behaves
 * as nsh_scalar_str_handler_ro(), i.e. with repect to the string length.
 * The callback function name will be based on the input parameter \a name,
 * which should be the same parameter used when registering the OID, i.e with
 * nsh_register_scalar_ro().
 */
#define nsh_scalar_group_handler_ro(name, type, id, callback, size, isstring) \
	_nsh_scalar_handler(name, type, id, callback, size, isstring, NULL)

/**
 * Create a read-write scalar OID handler.
 * @param name    OID define, without the \a oid_ prefix.
 * @param type    OID type.
 *                %ASN_COUNTER, %ASN_INTEGER, %ASN_TIMETICKS, %ASN_UNSIGNED,
 *                %ASN_IPADDRESS, %ASN_OCTET_STR or %ASN_OBJECT_ID.
 * @param get_cb  Callback function to get return value by a request.
 * @param size    Data length of handled value.
 * @param set_cb  Callback function to set value by a request.
 *
 * This function will create a read-write scalar handler callback function. A
 * read request to the OID object will return the value from the \a get_cb
 * callback function. A write request will pass the value to the \a set_cb
 * callback function. If a string value is to be handled, consider using
 * nsh_scalar_str_handler_rw() instead, else the string returned will always be
 * \a size long.
 * The callback function name will be based on the input parameter \a name,
 * which should be the same parameter used when registering the OID, i.e with
 * nsh_register_scalar_rw().
 */
#define nsh_scalar_handler_rw(name, type, get_cb, size, set_cb) \
	_nsh_scalar_handler(name, type, 0, get_cb, size, 0, set_cb)

/**
 * Create a read-write string OID handler.
 * @param name        OID define, without the \a oid_ prefix.
 * @param get_cb      Callback function to get string value by a request.
 * @param max_length  Maximum length of returned string.
 * @param set_cb      Callback function to set value by a request.
 *
 * This function will create a read-write string handler callback function. The
 * function is similar to nsh_scalar_handler_rw() except it will return a string
 * with the correct length, i.e. the actual string length instead of the maximum
 * length of the string.
 * The callback function name will be based on the input parameter \a name,
 * which should be the same parameter used when registering the OID, i.e with
 * nsh_register_scalar_rw().
 */
#define nsh_scalar_str_handler_rw(name, get_cb, max_length, set_cb) \
	_nsh_scalar_handler(name, ASN_OCTET_STR, 0, get_cb, max_length, 1, set_cb)

/**
 * Create a grouped read-write scalar OID handler.
 * @param name      OID define, without the \a oid_ prefix.
 * @param type      OID type.
 *                  %ASN_COUNTER, %ASN_INTEGER, %ASN_TIMETICKS, %ASN_UNSIGNED,
 *                  %ASN_IPADDRESS, %ASN_OCTET_STR or %ASN_OBJECT_ID.
 * @param id        Group identifier for this request.
 * @param get_cb    Callback function to get return value by a request.
 * @param size      Data length of handled value.
 * @param isstring  Set if value is a string.
 * @param set_cb    Callback function to set value by a request.
 *
 * This function will create a grouped read-write scalar handler callback
 * function. This function will behave in the same maner as
 * nsh_scalar_handler_rw() except that a \a id is used to distinguish different
 * OID requests. A common read callback, \a get_cb, and a common write callback,
 * \a set_cb, can therefore be shared between several created handlers. If the
 * \a isstring parameter is set, the function behaves as
 * nsh_scalar_str_handler_rw(), i.e. with repect to the string length.
 * The callback function name will be based on the input parameter \a name,
 * which should be the same parameter used when registering the OID, i.e with
 * nsh_register_scalar_rw().
 */
#define nsh_scalar_group_handler_rw(name, type, id, get_cb, size, isstring, set_cb) \
	_nsh_scalar_handler(name, type, id, get_cb, size, isstring, set_cb)

#endif /* NSH_H_ */

/** @example scalar_int.c */
/** @example scalar_ro.c */
/** @example scalar_str_ro.c */
/** @example scalar_group_ro.c */

/**
 * Local Variables:
 *  c-file-style: "linux"
 * End:
 */
