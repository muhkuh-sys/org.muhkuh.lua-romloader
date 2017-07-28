/***************************************************************************
 *   Based on libdcc/stdio.c from the OpenOCD project.                     *
 *   Copyright (C) 2008 by Dominic Rath                                    *
 *   Dominic.Rath@gmx.de                                                   *
 *   Copyright (C) 2008 by Spencer Oliver                                  *
 *   spen@spen-soft.co.uk                                                  *
 *   Copyright (C) 2008 by Frederik Kriewtz                                *
 *   frederik@kriewitz.eu                                                  *
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

#include "dcc_io.h"
#define TARGET_REQ_DEBUGMSG_ASCII 0x01

extern char ab_buffer;
extern char buffer_end;
extern char *pch_buffer_ptr;
extern void dbg_write(unsigned long dcc_data);

void dcc_buffer_flush()
{
    if (pch_buffer_ptr > &ab_buffer) 
    {
        char * pcMsg;
        unsigned int uiLen;
        unsigned long ulDcc_data;
        int iPos;
    
        uiLen = (pch_buffer_ptr - &ab_buffer) & 0xffff;
        dbg_write(TARGET_REQ_DEBUGMSG_ASCII | uiLen << 16);
        
        pcMsg = &ab_buffer;
        ulDcc_data = 0;
        iPos = 0;

        while (pcMsg < pch_buffer_ptr)
        {
            ulDcc_data |= ((unsigned long)*pcMsg) << iPos;
            ++pcMsg;
            iPos+=8;
            if ((iPos == 32) || (pcMsg == pch_buffer_ptr)) 
            {
                dbg_write(ulDcc_data);
                ulDcc_data = 0;
                iPos = 0;
            }
        }
    }
    else
    {
        dbg_write(TARGET_REQ_DEBUGMSG_ASCII);
    }
    
    pch_buffer_ptr = &ab_buffer;
}

void dcc_buffer_putchar(unsigned int iChar)
    {
        *pch_buffer_ptr = (char) iChar;
        ++pch_buffer_ptr;
        if (pch_buffer_ptr == &buffer_end)
            {
                dcc_buffer_flush();
            }
    }
