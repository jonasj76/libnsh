/* Private declarations
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

#define FN_HANDLE(_n) ( nsh_handle_ ## _n )
#define FN_INDEX(_n)  ( _n ## _index )

#define ENTRY_SIZE(s,e) sizeof(((s*)0)->e)

#ifndef UNUSED
#define UNUSED(x) UNUSED_ ## x __attribute__ ((unused))
#endif

int __nsh_scalar_handler(u_char type,
			 int id,
			 int (*get_cb)(void *val, int len, int id),
			 int get_sz,
			 long get_arg,
			 int (*set_cb)(void *val, int id),
			 netsnmp_mib_handler *UNUSED(handler),
			 netsnmp_handler_registration *UNUSED(reginfo),
			 netsnmp_agent_request_info *reqinfo,
			 netsnmp_request_info *requests);

/**
 * Local Variables:
 *  c-file-style: "linux"
 * End:
 */
