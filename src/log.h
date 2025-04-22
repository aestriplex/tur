/* log.h
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

#ifndef __LOG_H__
#define __LOG_H__

#include "codes.h"

#define STDERR_FILENO 2

return_code_t init_info_logger(int file_descriptor);
return_code_t init_error_logger(int file_descriptor);
return_code_t init_def_info_logger(void);
return_code_t init_def_error_logger(void);
return_code_t init_default_loggers(void);
return_code_t log_info(const char *format, ...);
return_code_t log_err(const char *format, ...);

#endif /* __LOG_H__ */
