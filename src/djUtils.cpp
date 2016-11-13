//---------------------------------------------------------------------------
// djUtils.cpp
//
// Copyright (C) 2002-2007 David Joffe
// http://grayskygames.com/
//---------------------------------------------------------------------------

#include "djUtils.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#include <io.h> // _finddata_t
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

char *djStrDup(const char *szSource)
{
	if (szSource)
	{
		char *szRet = new char[strlen(szSource)+1];
		strcpy(szRet, szSource);
		return szRet;
	}
	return NULL;
}

FILE *g_pLog = NULL;

void djLog(const char *szFormat, ...)
{
	// Log file only in debug mode
#ifdef _DEBUG
	if (g_pLog==NULL)
	{
		g_pLog = fopen("syasokobanlog.txt", "w");
	}
	char szBuf[1024];
	va_list arg;
	va_start(arg, szFormat);
	vsprintf(szBuf, szFormat, arg);
	strcat(szBuf, "\n");
	va_end(arg);
#ifdef WIN32
	::OutputDebugString(szBuf);
#endif
	if (g_pLog)
	{
		fprintf(g_pLog, "%s", szBuf);
		fflush(g_pLog);
	}
#endif
}

void djAssert(int nExpression, const char *szFile, int nLine, const char *szExpression)
{
	if (nExpression==0)
	{
		char szMsg[16384];
		sprintf(szMsg, "Assertion failed (%s) in file %s at line %d", szExpression, szFile, nLine);
		djLog("ASSERT: [%s]", szMsg);
		#ifdef WIN32
			// Save assertion to an assertion log file
			FILE *pOut = fopen("Log_Assertions.txt", "a");
			if (pOut)
			{
				fprintf(pOut, "%s\n", szMsg);
				fclose(pOut);
			}

			// Tell user about this
			::MessageBox(NULL, szMsg, "Program error!", MB_OK);
		#else
			printf("Error: %s", szMsg);
		#endif

		// fixme[h]: application needs a chance to handle this, e.g. to clean up SDL

#ifdef _DEBUG
		abort();
#else
		exit(-1);
#endif
	}
}

#ifdef WIN32

int djStrCaseCmp(const char *sz1, const char *sz2)
{
	return stricmp(sz1, sz2);
}

void djFindFiles(const char *szDirectory, const char *szWildcard, std::vector<std::string> &asFiles)
{
	std::string s;
	if (szDirectory) s = szDirectory;
	s += szWildcard;
	_finddata_t FD;
	long hFile = _findfirst(s.c_str(), &FD);
	if (hFile==-1) // Error
		return;
	while( hFile != -1L )
	{
		asFiles.push_back(FD.name);

		if ( _findnext( hFile,  &FD ) == -1L )
			break;
	}

	_findclose(hFile);
}

bool djFileExists(const char *szFilename)
{
	_finddata_t FD;
	long hFile = _findfirst(szFilename, &FD);
	if (hFile==-1L)
		return false;
	_findclose(hFile);
	return true;
}

#else

int djStrCaseCmp(const char *sz1, const char *sz2)
{
	return strcasecmp(sz1, sz2);
}

void djFindFiles(const char *szDirectory, const char *szWildcard, std::vector<std::string> &asFiles)
{
	DIR *dir = opendir(szDirectory ? szDirectory : ".");
	if (!dir)
	{
		djLog("Error opening directory %s", szDirectory);
		fflush(NULL);
		return;
	}

	struct dirent* entry = (dirent *)NULL;
	while (entry = readdir(dir))
	{
		if (entry->d_name)
		{
//			// Check if the given entry is a directory
//			if (stat(entry->d_name, &dir_stat) == -1)
//				continue;
			if (strstr(entry->d_name, szWildcard))
				asFiles.push_back(entry->d_name);
		}
	}
}

bool djFileExists(const char *szFilename)
{
	struct stat sb;
	if (stat(szFilename, &sb) == -1)
		return false;
	return true;
}

#endif

/*
bool djFileExists(const char *szFilename)
{
	std::vector<std::string> asFiles;
	djFindFiles(NULL, szFilename, asFiles);
	if (asFiles.size()>0)
		return true;
	return false;
}
*/

void djStripCRLF(char *buf)
{
	// While last character in string is either CR or LF, remove.
	while (strlen(buf)>=1 && ((buf[strlen(buf)-1]=='\r') || (buf[strlen(buf)-1]=='\n')))
		buf[strlen(buf)-1] = '\0';
}
