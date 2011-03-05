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


#define MAX_COMMANDLINE_ARGUMENT 4096


capture_std::capture_std(void)
 : m_tCaptureThread(-1)
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
	sizTable = lua_objlen(ptLuaState, iIndex);
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
				else if( sizString>MAX_COMMANDLINE_ARGUMENT )
				{
					/* The argument exceeds the maximum size. */
					iResult = -1;
				}
				else
				{
					/* Add the string to the array. */

					pcString = (char*)malloc(sizString);
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


#define PIPE_READ_END 0
#define PIPE_WRITE_END 1

int capture_std::run(const char *pcCommand, lua_State *ptLuaStateForTableAccess)
{
	int iResult;
	char **ppcCmdArguments;
	int aiPipeOutFd[2];
	int aiPipeErrFd[2];
	pid_t tPidExec;
	pid_t tPidCapture;
	ssize_t ssizRead;
	unsigned char aucBuffer[4096];
	int iNewFd;
	fd_set tOpenFdSet;
	fd_set tReadFdSet;
	int iMaxFd;
	int iSelect;


	/* Init all variables. */
	aiPipeOutFd[PIPE_READ_END] = -1;
	aiPipeOutFd[PIPE_WRITE_END] = -1;
	aiPipeErrFd[PIPE_READ_END] = -1;
	aiPipeErrFd[PIPE_WRITE_END] = -1;
	tPidExec = -1;
	tPidCapture = -1;

	/* The third argument of the function is the table. This is stack index 3. */
	ppcCmdArguments = get_strings_from_table(3, ptLuaStateForTableAccess);
	if( ppcCmdArguments==NULL )
	{
		iResult = 0;
	}
	else
	{
		/* Create the pipes. */
		if( pipe(aiPipeOutFd)!=0 )
		{
			fprintf(stderr, "failed to create out pipe (%d): %s\n", errno, strerror(errno));
			iResult = -1;
		}
		else if( pipe(aiPipeErrFd)!=0 )
		{
			fprintf(stderr, "failed to create err pipe (%d): %s\n", errno, strerror(errno));
			iResult = -1;
		}
		else
		{
			/* Create the exec thread. */
			tPidExec = fork();
			if( tPidExec==-1 )
			{
				fprintf(stderr, "Failed to create the exec thread: %s\n", strerror(errno));
				iResult = -1;
			}
			else if( tPidExec==0 )
			{
				/* This is the exec thread. */

				/* The exec thread only writes to both pipes. Close the read end. */
				close(aiPipeOutFd[PIPE_READ_END]);
				close(aiPipeErrFd[PIPE_READ_END]);

				/* Dup the write end of the pipes to stdout and stderr. */
				iNewFd = dup2(aiPipeOutFd[PIPE_WRITE_END], STDOUT_FILENO);
				if( iNewFd!=-1 )
				{
					iNewFd = dup2(aiPipeErrFd[PIPE_WRITE_END], STDERR_FILENO);
					if( iNewFd!=-1 )
					{
						write(STDOUT_FILENO, "This is exec stdout.\n", 22);
						write(STDERR_FILENO, "This is exec stderr.\n", 22);


						/* Run the command. */
						iResult = execv(pcCommand, ppcCmdArguments);

						/* If this part is reached, the exec command failed. */
						fprintf(stderr, "execv returned with errorcode %d.\n", iResult);
					}
				}

				close(aiPipeOutFd[PIPE_WRITE_END]);
				close(aiPipeErrFd[PIPE_WRITE_END]);

				exit(EXIT_FAILURE);
			}
			else
			{
				/* Close the write end of both pipes. */
				close(aiPipeOutFd[PIPE_WRITE_END]);
				close(aiPipeErrFd[PIPE_WRITE_END]);

				/* This is the parent. */
				m_tExecThread = tPidExec;


				printf("*** Start Capture ***\n");
				fprintf(stdout, "This is capture stdout.\n");
				fprintf(stderr, "This is capture stderr.\n");

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

					iMaxFd = aiPipeOutFd[PIPE_READ_END];
					if( iMaxFd<aiPipeErrFd[PIPE_READ_END] )
					{
						iMaxFd = aiPipeErrFd[PIPE_READ_END];
					}
					++iMaxFd;
					printf("aiPipeOutFd[PIPE_READ_END] = %d\n", aiPipeOutFd[PIPE_READ_END]);
					printf("aiPipeErrFd[PIPE_READ_END] = %d\n", aiPipeErrFd[PIPE_READ_END]);
					printf("maxfd = %d\n", iMaxFd);


					FD_ZERO(&tOpenFdSet);
					FD_SET(aiPipeOutFd[PIPE_READ_END], &tOpenFdSet);
					FD_SET(aiPipeErrFd[PIPE_READ_END], &tOpenFdSet);

					do
					{
						FD_ZERO(&tReadFdSet);
						if( FD_ISSET(aiPipeOutFd[PIPE_READ_END], &tOpenFdSet) )
						{
							FD_SET(aiPipeOutFd[PIPE_READ_END], &tReadFdSet);
						}
						if( FD_ISSET(aiPipeErrFd[PIPE_READ_END], &tOpenFdSet) )
						{
							FD_SET(aiPipeErrFd[PIPE_READ_END], &tReadFdSet);
						}

						iSelect = select(iMaxFd, &tReadFdSet, NULL, NULL, NULL);
						printf("select: %d\n", iSelect);
						if( iSelect==-1 )
						{
							/* Select failed! */
							iResult = EXIT_FAILURE;
							break;
						}
						else if( iSelect>0 )
						{
							if( FD_ISSET(aiPipeOutFd[PIPE_READ_END], &tReadFdSet) )
							{
								ssizRead = read(aiPipeOutFd[PIPE_READ_END], aucBuffer, sizeof(aucBuffer)-1);
								if( ssizRead<0 )
								{
									iResult = EXIT_FAILURE;
									break;
								}
								else if( ssizRead==0 )
								{
									FD_CLR(aiPipeOutFd[PIPE_READ_END], &tOpenFdSet);
								}
								else
								{
									aucBuffer[ssizRead] = 0;
									printf("<OUT len=%d>%s</OUT>\n", ssizRead, aucBuffer);
								}
							}
							if( FD_ISSET(aiPipeErrFd[PIPE_READ_END], &tReadFdSet) )
							{
								ssizRead = read(aiPipeErrFd[PIPE_READ_END], aucBuffer, sizeof(aucBuffer)-1);
								if( ssizRead<0 )
								{
									iResult = EXIT_FAILURE;
									break;
								}
								else if( ssizRead==0 )
								{
									FD_CLR(aiPipeErrFd[PIPE_READ_END], &tOpenFdSet);
								}
								else
								{
									aucBuffer[ssizRead] = 0;
									printf("<ERR len=%d>%s</ERR>\n", ssizRead, aucBuffer);
								}
							}
						}
					} while( (FD_ISSET(aiPipeOutFd[PIPE_READ_END], &tOpenFdSet)) || (FD_ISSET(aiPipeErrFd[PIPE_READ_END], &tOpenFdSet)) );

					close(aiPipeOutFd[PIPE_READ_END]);
					close(aiPipeErrFd[PIPE_READ_END]);

					printf("*** Stop Capture ***\n");

					exit(iResult);
				}
				else
				{
					/* This is the parent. */
					m_tCaptureThread = tPidCapture;


					printf("wait until the threads finished.\n");
					waitpid(m_tExecThread, NULL, 0);
					waitpid(m_tCaptureThread, NULL, 0);
					printf("ok, all finished!\n");
				}
			}
		}

		free_string_table(ppcCmdArguments);
	}
	return 0;
}

