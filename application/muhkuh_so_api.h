/***************************************************************************
 *   Copyright (C) 2010 by Christoph Thelen                                *
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


/* Is the import/export symbot defined already? */
#if !defined(MUHKUH_SO_API)
	/* No, it is not defined yet. */

	/* The import/export symbol is only needed on windows hosts.
	 * Is this a windows host?
	 */
#       if defined(WIN32)
		/* Yes, this is a windows host. */

		/* Is this the shared object? */
#               if defined(__BUILD_SHARED_OBJECT__)
			/* Yes, it is the shared object -> export the
			 * symbols.
			 */
#                       define MUHKUH_SO_API __declspec(dllexport)
#               else
			/* No, it is the main application -> the symbols need
			 * to be imported from the dll.
			 */
#                       define MUHKUH_SO_API __declspec(dllimport)
#               endif
#       else
		/* This is no windows host. Keep the define empty. */
#               define MUHKUH_SO_API
#       endif
#endif
