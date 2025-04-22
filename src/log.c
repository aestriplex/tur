/* log.c
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

#include "codes.h"
#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static FILE *logger = NULL;

return_code_t init_logger(int file_descriptor)
{
	FILE *_logger = fdopen(file_descriptor, "w");
	if (!_logger) { return INVALID_LOGGER_FD; }
	logger = _logger;
	return OK;
}

return_code_t init_default_logger(void)
{
	logger = stderr;
	return OK;
}

return_code_t log_info(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int res = vfprintf(logger, format, args);
	va_end(args);
	return res < 0 ? RUNTIME_LOGGER_ERROR : OK;
}
