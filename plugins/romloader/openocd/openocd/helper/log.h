/***************************************************************************
 *   Copyright (C) 2005 by Dominic Rath                                    *
 *   Dominic.Rath@gmx.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef ERROR_H
#define ERROR_H

#include "replacements.h"
#include "command.h"

#include <stdarg.h>

/* logging priorities 
 * LOG_ERROR - fatal errors, that are likely to cause program abort
 * LOG_WARNING - non-fatal errors, that may be resolved later
 * LOG_INFO - state information, etc.
 * LOG_DEBUG - debug statements, execution trace
 */
enum log_levels
{
	LOG_ERROR = 0,
	LOG_WARNING = 1,
	LOG_INFO = 2,
	LOG_DEBUG = 3
};

typedef void (*pfn_log_printf_t)(enum log_levels level, const char *format, ...);
typedef void (*pfn_short_log_printf_t)(enum log_levels level, const char *format, ...);

extern pfn_log_printf_t pfn_log_printf;
extern pfn_short_log_printf_t pfn_short_log_printf;

extern void log_debug(const char *format, ...);
extern void log_info(const char *format, ...);
extern void log_warning(const char *format, ...);
extern void log_error(const char *format, ...);

extern void slog_debug(const char *format, ...);
extern void slog_info(const char *format, ...);
extern void slog_warning(const char *format, ...);
extern void slog_error(const char *format, ...);

extern void default_log_printf(enum log_levels level, const char *format, ...);
extern void default_short_log_printf(enum log_levels level, const char *format, ...);

extern void log_set_output_handler(pfn_log_printf_t pfn_log, pfn_short_log_printf_t pfn_short_log);
extern int log_register_commands(struct command_context_s *cmd_ctx);
extern int log_init(struct command_context_s *cmd_ctx);

extern int debug_level;

/* use 4 functions instead of vararg defines until I figure out how to do this with msvc */
#define DEBUG		log_debug
#define INFO		log_info
#define WARNING		log_warning
#define ERROR		log_error

#define SDEBUG		slog_debug
#define SINFO		slog_info
#define SWARNING	slog_warning
#define SERROR		slog_error

/* general failures
 * error codes < 100
 */
#define ERROR_OK					(0)
#define ERROR_INVALID_ARGUMENTS		(-1)
#define ERROR_NO_CONFIG_FILE		(-2)
#define ERROR_BUF_TOO_SMALL			(-3)

#endif /* ERROR_H */
