/* codes.h
 * -----------------------------------------------------------------------
 * Copyright (C) 2025  Matteo Nicoli
 *
 * This file is part of TUR.
 *
 * TUR is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __CODES_H__
#define __CODES_H__

#define __TUR_VERSION__ "1.0.0"

typedef enum {
	OK                            = 0x00,
	INVALID_REPOS_LIST_PATH       = 0x01,
	ARRAY_RESIZE_ALLOCATION_ERROR = 0x02,
	INDEX_ALLOCATION_ERROR        = 0x03,
	PARENT_COMMIT_UNAVAILBLE      = 0x04,
	COMPARE_TREES_ERROR           = 0x05,
	CANNOT_RETRIEVE_STATS         = 0x06,
	REQUIRED_ARG_NULL             = 0x07,
	INT_OVERFLOW                  = 0x08,
	USUPPORTED_NEGATIVE_VALUE     = 0x09,
	UNSUPPORTED_VALUE             = 0x0A,
	UNKONWN_SORT_ORDER            = 0x0B,
	NULL_PARAMETER                = 0x0C,

	RUNTIME_THREAD_CREATE_ERROR   = 0xFE,
	RUNTIME_MALLOC_ERROR          = 0xFF
} return_code_t;

#endif /* __CODES_H__ */
