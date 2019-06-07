/***************************************************************************
 *   Copyright (C) 2019 by Christoph Thelen                                *
 *   doc_bacardi@users.sourceforge.net                                     *
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

#include <stdarg.h>
#include <stdint.h>

#ifndef __MUHKUH_LOG_H__
#define __MUHKUH_LOG_H__

/*-----------------------------------*/

#if defined(_WINDOWS)
#       define MUHKUH_EXPORT __declspec(dllexport)
#else
#       define MUHKUH_EXPORT
#endif

/*-----------------------------------*/

class MUHKUH_EXPORT muhkuh_log
{
public:
	muhkuh_log(void);

	typedef enum MUHKUH_LOG_LEVEL_ENUM
	{
		MUHKUH_LOG_LEVEL_EMERG     = 1,
		MUHKUH_LOG_LEVEL_ALERT     = 2,
		MUHKUH_LOG_LEVEL_FATAL     = 3,
		MUHKUH_LOG_LEVEL_ERROR     = 4,
		MUHKUH_LOG_LEVEL_WARNING   = 5,
		MUHKUH_LOG_LEVEL_NOTICE    = 6,
		MUHKUH_LOG_LEVEL_INFO      = 7,
		MUHKUH_LOG_LEVEL_DEBUG     = 8,
		MUHKUH_LOG_LEVEL_TRACE     = 9
	} MUHKUH_LOG_LEVEL_T;

	void emerg(const char *pcFormat, ...) /* __attribute__ ((format (printf, 1, 2))) */;
	void alert(const char *pcFormat, ...) /* __attribute__ ((format (printf, 1, 2))) */;
	void fatal(const char *pcFormat, ...) /* __attribute__ ((format (printf, 1, 2))) */;
	void error(const char *pcFormat, ...) /* __attribute__ ((format (printf, 1, 2))) */;
	void warning(const char *pcFormat, ...) /* __attribute__ ((format (printf, 1, 2))) */;
	void notice(const char *pcFormat, ...) /* __attribute__ ((format (printf, 1, 2))) */;
	void info(const char *pcFormat, ...) /* __attribute__ ((format (printf, 1, 2))) */;
	void debug(const char *pcFormat, ...) /* __attribute__ ((format (printf, 1, 2))) */;
	void trace(const char *pcFormat, ...) /* __attribute__ ((format (printf, 1, 2))) */;
	void log(MUHKUH_LOG_LEVEL_T tLevel, const char *pcFormat, ...) /* __attribute__ ((format (printf, 1, 2))) */;
	void hexdump(MUHKUH_LOG_LEVEL_T tLevel, const uint8_t *pucData, uint32_t ulSize);

private:
	void vlog(MUHKUH_LOG_LEVEL_T tLevel, const char *pcFormat, va_list argptr);
	void slog(MUHKUH_LOG_LEVEL_T tLevel, const char *pcMessage);
};


/*-----------------------------------*/

#endif  /* __MUHKUH_LOG_H__ */
