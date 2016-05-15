/* Deprecated functions
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

#ifndef NSH_DEPRECATED_H_
#define NSH_DEPRECATED_H_

int nsh_register_table(const char*              name,
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
		       int                      access);

#endif /* NSH_DEPRECATED_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 * End:
 */
