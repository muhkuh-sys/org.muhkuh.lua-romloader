/***************************************************************************
 *   Copyright (C) 2008 by Christoph Thelen                                *
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


#ifndef __SERIAL_VECTORS_H__
#define __SERIAL_VECTORS_H__

/* The V1 vectors are the interface to the hardware. They send data to the PC
   and receive data from there.
*/

#if ASIC_TYP==10
	typedef unsigned int (*PFN_SERIAL_V1_GET_T)(unsigned int tHandle);
	typedef void         (*PFN_SERIAL_V1_PUT_T)(unsigned int tHandle, unsigned char ucChar);
	typedef unsigned int (*PFN_SERIAL_V1_PEEK_T)(unsigned int tHandle);
	typedef void         (*PFN_SERIAL_V1_FLUSH_T)(unsigned int tHandle);

#       define SERIAL_V1_GET() tSerialV1Vectors.fn.fnGet(0)
#       define SERIAL_V1_PUT(c) tSerialV1Vectors.fn.fnPut(0, c)
#       define SERIAL_V1_PEEK() tSerialV1Vectors.fn.fnPeek(0)
#       define SERIAL_V1_FLUSH() tSerialV1Vectors.fn.fnFlush(0)
#else
	typedef unsigned char (*PFN_SERIAL_V1_GET_T)(void);
	typedef void          (*PFN_SERIAL_V1_PUT_T)(unsigned int iChar);
	typedef unsigned int  (*PFN_SERIAL_V1_PEEK_T)(void);
	typedef void          (*PFN_SERIAL_V1_FLUSH_T)(void);

#       define SERIAL_V1_GET() tSerialV1Vectors.fn.fnGet()
#       define SERIAL_V1_PUT(c) tSerialV1Vectors.fn.fnPut(c)
#       define SERIAL_V1_PEEK() tSerialV1Vectors.fn.fnPeek()
#       define SERIAL_V1_FLUSH() tSerialV1Vectors.fn.fnFlush()
#endif

typedef struct
{
	PFN_SERIAL_V1_GET_T fnGet;
	PFN_SERIAL_V1_PUT_T fnPut;
	PFN_SERIAL_V1_PEEK_T fnPeek;
	PFN_SERIAL_V1_FLUSH_T fnFlush;
} SERIAL_V1_COMM_FN_T;

typedef union
{
	SERIAL_V1_COMM_FN_T fn;
	unsigned long aul[4];
} SERIAL_V1_COMM_UI_FN_T;
extern SERIAL_V1_COMM_UI_FN_T tSerialV1Vectors;



/* The V2 vectors are the interface to a program executed with the 'call'
   command.
*/

typedef unsigned char (*PFN_SERIAL_V2_GET_T) (void);
typedef void          (*PFN_SERIAL_V2_PUT_T) (unsigned int iChar);
typedef unsigned int  (*PFN_SERIAL_V2_PEEK_T) (void);
typedef void          (*PFN_SERIAL_V2_FLUSH_T) (void);

#define SERIAL_V2_GET() tSerialV2Vectors.fn.fnGet()
#define SERIAL_V2_PUT(c) tSerialV2Vectors.fn.fnPut(c)
#define SERIAL_V2_PEEK() tSerialV2Vectors.fn.fnPeek()
#define SERIAL_V2_FLUSH() tSerialV2Vectors.fn.fnFlush()

typedef struct
{
	PFN_SERIAL_V2_GET_T fnGet;
	PFN_SERIAL_V2_PUT_T fnPut;
	PFN_SERIAL_V2_PEEK_T fnPeek;
	PFN_SERIAL_V2_FLUSH_T fnFlush;
} SERIAL_V2_COMM_FN_T;

typedef union
{
	SERIAL_V2_COMM_FN_T fn;
	unsigned long aul[4];
} SERIAL_V2_COMM_UI_FN_T;
extern SERIAL_V2_COMM_UI_FN_T tSerialV2Vectors;


#endif	/* __SERIAL_VECTORS_H__ */


