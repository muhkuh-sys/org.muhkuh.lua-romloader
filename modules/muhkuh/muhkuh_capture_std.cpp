/***************************************************************************
 *   Copyright (C) 2011 by Christoph Thelen                                *
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


#include "muhkuh_capture_std.h"

#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>


#include <wx/wx.h>
#include <wx/process.h>



#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 501
#       define WRAPPER_LUA_RAWLEN lua_strlen
#elif LUA_VERSION_NUM == 501
#       define WRAPPER_LUA_RAWLEN lua_objlen
#elif LUA_VERSION_NUM == 502
#       define WRAPPER_LUA_RAWLEN lua_rawlen
#endif



capture_std::capture_std(long lMyId, long lEvtHandlerId)
 : m_lMyId(lMyId)
 , m_lEvtHandlerId(lEvtHandlerId)
 , m_tCaptureThread(-1)
 , m_tExecThread(-1)
{
}


capture_std::~capture_std(void)
{
}





char **capture_std::get_strings_from_table(int iIndex, lua_State *ptLuaState) const
{
	int iResult;
	size_t sizTable;
	size_t sizEntryCnt;
	char **ppcStrings;
	const char *pcLuaString;
	char *pcString;
	size_t sizString;


	/* Be optimistic. */
	iResult = 0;

	/* get the size of the table */
	/* NOTE: lua_rawlen is new in LUA5.2, but swig provides a
	 * compatibility wrapper in swigluarun.h .
	 */
	sizTable = WRAPPER_LUA_RAWLEN(ptLuaState, iIndex);
	printf("The table has %d elements.\n", sizTable);

	/* Allocate the array for the strings and one terminating NULL. */
	ppcStrings = (char**)malloc(sizeof(char*)*(sizTable+1));
	if( ppcStrings==NULL )
	{
		/* Failed to allocate the array. */
		iResult = -1;
	}
	else
	{
		/* Clear the complete array with 0. */
		memset(ppcStrings, 0, sizeof(char*)*(sizTable+1));

		/* Loop over all table elements and add them to the array. */
		sizEntryCnt = 0;
		while( iResult==0 && sizEntryCnt<sizTable )
		{
			printf("Get entry %d\n", sizEntryCnt+1);
			/* Push the table entry on the stack. */
			lua_rawgeti(ptLuaState, iIndex, sizEntryCnt+1);
			/* Is this a string or a number? */
			if( lua_isstring(ptLuaState, -1)!=1 )
			{
				/* No String, cancel. */
				iResult = -1;
			}
			else
			{
				pcLuaString = lua_tolstring(ptLuaState, -1, &sizString);
				if( pcLuaString==NULL )
				{
					/* Failed to convert the data to a string. */
					iResult = -1;
				}
				else if( sizString+1>c_iMaxCommandlineArgument )
				{
					/* The argument exceeds the maximum size. */
					iResult = -1;
				}
				else
				{
					/* Add the string to the array. */

					pcString = (char*)malloc(sizString+1);
					/* Copy the string data and the terminating '\0' to the new buffer. */
					memcpy(pcString, pcLuaString, sizString+1);
					ppcStrings[sizEntryCnt] = pcString;
					printf("ppcStrings[%d] = '%s'\n", sizEntryCnt, pcString);
				}
			}

			/* Pop one element from the stack. */
			lua_pop(ptLuaState, 1);

			++sizEntryCnt;
		}

		/* Free the string array if something went wrong. */
		if( iResult!=0 )
		{
			free_string_table(ppcStrings);
			ppcStrings = NULL;
		}
	}

	return ppcStrings;
}


int capture_std::free_string_table(char **ppcTable) const
{
	char **ppcCnt;
	char *pcString;


	if( ppcTable!=NULL )
	{
		ppcCnt = ppcTable;
		while( *ppcCnt!=NULL )
		{
			free(*ppcCnt);
			++ppcCnt;
		}
		free(ppcTable);
	}
}


void capture_std::send_finished_event(int iPid, int iResult)
{
	wxWindow *ptHandlerWindow;
	wxProcessEvent tProcessEvent(m_lMyId, iPid, iResult);


	if( m_lEvtHandlerId!=wxID_ANY )
	{
		ptHandlerWindow = wxWindow::FindWindowById(m_lEvtHandlerId, NULL);
		if( ptHandlerWindow!=NULL )
		{
			ptHandlerWindow->AddPendingEvent(tProcessEvent);
		}
	}
}


int capture_std::get_pty(void)
{
	int iResult;


	/* Get an unused pseudo-terminal master device. */
	m_iFdPtyMaster = posix_openpt(O_RDWR);
	if( m_iFdPtyMaster<0 )
	{
		fprintf(stderr, "Failed to get PTY master: %d %s\n", errno, strerror(errno));
		iResult = -1;
	}
	else
	{
		printf("iFdPtyMaster = %d\n", m_iFdPtyMaster);
		iResult = grantpt(m_iFdPtyMaster);
		if( iResult!=0 )
		{
			fprintf(stderr, "grantpt failed: %d %s\n", errno, strerror(errno));
		}
		else
		{
			iResult = unlockpt(m_iFdPtyMaster);
			if( iResult!=0 )
			{
				fprintf(stderr, "unlockpt failed: %d %s\n", errno, strerror(errno));
			}
			else
			{
				/* Get the name of the pty. */
				iResult = ptsname_r(m_iFdPtyMaster, m_acPtsName, c_iMaxPtsName);
				if( iResult!=0 )
				{
					fprintf(stderr, "ptsname_r failed: %d %s\n", errno, strerror(errno));
				}
				else
				{
					printf("pts name: '%s'\n", m_acPtsName);
				}
			}
		}
	}

	return iResult;
}


int capture_std::start_exec_thread(const char *pcCommand, char **ppcCmdArguments)
{
	pid_t tPidExec;
	int iPtyFd;
	int iResult;
	char **ppcCnt;


	tPidExec = fork();
	if( tPidExec==-1 )
	{
		fprintf(stderr, "Failed to create the exec thread: %s\n", strerror(errno));
		return -1;
	}
	else if( tPidExec==0 )
	{
		/* This is the exec thread. */
		fprintf(stderr, "exec_thread start\n");
		
		/* Open the slave pty in write mode. */
		iPtyFd = open(m_acPtsName, O_WRONLY);
		if( iPtyFd<0 )
		{
			fprintf(stderr, "Failed to open the pseudo terminal: (%d) %s\n", errno, strerror(errno));
		}
		else
		{
			/* Dup the pty to stdout. */
			if( dup2(iPtyFd, STDOUT_FILENO)==-1 )
			{
				fprintf(stderr, "Failed to connect stdout with the pseudo terminal: (%d) %s\n", errno, strerror(errno));
			}
			else if( dup2(iPtyFd, STDERR_FILENO)==-1 )
			{
				fprintf(stderr, "Failed to connect stderr with the pseudo terminal: (%d) %s\n", errno, strerror(errno));
			}
			else
			{
				/* Run the command. */
				iResult = execv(pcCommand, ppcCmdArguments);

				/* If this part is reached, the exec command failed. */
				fprintf(stderr, "Command execution failed with errorcode %d.\n", iResult);
				fprintf(stderr, "Command:   '%s'\n", pcCommand);
				fprintf(stderr, "Arguments:");
				ppcCnt = ppcCmdArguments;
				while( *ppcCnt!=NULL )
				{
					fprintf(stderr, " '%s'", *ppcCnt);
					++ppcCnt;
				}
				fprintf(stderr, "\n");
			}

			close(iPtyFd);
		}

		fprintf(stderr, "exec_thread finish\n");

		exit(EXIT_FAILURE);
	}
	else
	{
		/* This is the parent thread. */
		m_tExecThread = tPidExec;
		return 0;
	}
}


int capture_std::run(const char *pcCommand, lua_State *ptLuaStateForTableAccess)
{
	int iResult;
	char **ppcCmdArguments;
	pid_t tPidCapture;
	ssize_t ssizRead;
	unsigned char aucBuffer[4096];
	int iNewFd;
	fd_set tReadFdSet;
	int iStatus;
	int iThreadResult;


	/* Init all variables. */
	tPidCapture = -1;

	/* The third argument of the function is the table. This is stack index 3. */
	ppcCmdArguments = get_strings_from_table(3, ptLuaStateForTableAccess);
	if( ppcCmdArguments==NULL )
	{
		iResult = 0;
	}
	else
	{
		/* Create the pty. */
		iResult = get_pty();
		if( iResult==0 )
		{
			/* Create the exec thread. */
			iResult = start_exec_thread(pcCommand, ppcCmdArguments);
			if( iResult==0 )
			{
				printf("*** Start Capture ***\n");

				tPidCapture = fork();
				if( tPidCapture==-1 )
				{
					fprintf(stderr, "Failed to create the capture thread (%d): %s\n", errno, strerror(errno));
					iResult = -1;
				}
				else if( tPidCapture==0 )
				{
					/* This is the capture thread. */
					printf("This is the capture thread.\n");

					do
					{
						FD_ZERO(&tReadFdSet);
						FD_SET(m_iFdPtyMaster, &tReadFdSet);

						iStatus = select(m_iFdPtyMaster+1, &tReadFdSet, NULL, NULL, NULL);
						if( iStatus==-1 )
						{
							/* Select failed! */
							iResult = EXIT_FAILURE;
							break;
						}
						else if( iStatus>0 )
						{
							ssizRead = read(m_iFdPtyMaster, aucBuffer, sizeof(aucBuffer)-1);
							if( ssizRead<0 )
							{
								if( errno==EIO )
								{
									/* The terminal was closed. */
									iResult = EXIT_SUCCESS;
								}
								else
								{
									fprintf(stderr, "read error %d %s\n", errno, strerror(errno));
									iResult = EXIT_FAILURE;
								}
								break;
							}
							else
							{
								aucBuffer[ssizRead] = 0;
								printf("<OUT len=%d>%s</OUT>\n", ssizRead, aucBuffer);
							}
						}
					} while( iStatus>0 );

					close(m_iFdPtyMaster);

					printf("*** Stop Capture ***\n");

					iStatus = waitpid(m_tExecThread, NULL, 0);
					if( WIFEXITED(iStatus) )
					{
						iThreadResult = WEXITSTATUS(iStatus);
					}
					else
					{
						iThreadResult = -1;
					}

					/* Send an event to the handler. */
//					send_finished_event(m_tExecThread, iThreadResult);

					exit(iResult);
				}
				else
				{
					/* This is the parent. */
					m_tCaptureThread = tPidCapture;


					printf("wait until the threads finished.\n");
					waitpid(m_tCaptureThread, NULL, 0);
					printf("ok, all finished!\n");
				}
			}
		}

		free_string_table(ppcCmdArguments);
	}
	return 0;
}

