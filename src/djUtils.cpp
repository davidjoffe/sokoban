//---------------------------------------------------------------------------
// djUtils.cpp
//
// Copyright (C) 2002-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "djUtils.h"
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <filesystem>
#include <iostream>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace fs = std::filesystem;

#ifdef _DEBUG
static std::mutex g_logMutex;
static FILE *g_pLog = nullptr;
#endif

void djLog([[maybe_unused]] const char *szFormat, ...)
{
#ifdef _DEBUG
	std::lock_guard<std::mutex> lock(g_logMutex);
	if (!g_pLog)
	{
		g_pLog = fopen("syasokobanlog.txt", "w");
	}
	char szBuf[2048];
	va_list arg;
	va_start(arg, szFormat);
	vsnprintf(szBuf, sizeof(szBuf), szFormat, arg);
	va_end(arg);

#ifdef WIN32
	::OutputDebugStringA(szBuf);
	::OutputDebugStringA("\n");
#endif
	if (g_pLog)
	{
		fprintf(g_pLog, "%s\n", szBuf);
		fflush(g_pLog);
	}
#endif
}

void djAssert(bool bExpression, const char *szFile, int nLine, const char *szExpression)
{
	if (!bExpression)
	{
		char szMsg[4096];
		snprintf(szMsg, sizeof(szMsg), "Assertion failed (%s) in file %s at line %d", szExpression, szFile, nLine);
		djLog("ASSERT: [%s]", szMsg);

#ifdef WIN32
		FILE *pOut = fopen("Log_Assertions.txt", "a");
		if (pOut)
		{
			fprintf(pOut, "%s\n", szMsg);
			fclose(pOut);
		}
		::MessageBoxA(nullptr, szMsg, "Program error!", MB_OK | MB_ICONERROR);
#else
		std::cerr << "Error: " << szMsg << '\n';
#endif

#ifdef _DEBUG
		std::abort();
#else
		std::exit(-1);
#endif
	}
}

int djStrCaseCmp(std::string_view s1, std::string_view s2)
{
	auto it1 = s1.begin();
	auto it2 = s2.begin();
	while (it1 != s1.end() && it2 != s2.end())
	{
		int c1 = std::tolower(static_cast<unsigned char>(*it1));
		int c2 = std::tolower(static_cast<unsigned char>(*it2));
		if (c1 != c2)
			return c1 - c2;
		++it1;
		++it2;
	}
	if (s1.size() < s2.size()) return -1;
	if (s1.size() > s2.size()) return 1;
	return 0;
}

void djFindFiles(const std::string &sDirectory, const std::string &sWildcard, std::vector<std::string> &asFiles)
{
	std::error_code ec;
	fs::path dirPath = sDirectory.empty() ? fs::current_path() : fs::path(sDirectory);

	if (!fs::exists(dirPath, ec) || !fs::is_directory(dirPath, ec))
		return;

	for (const auto &entry : fs::directory_iterator(dirPath, ec))
	{
		if (entry.is_regular_file(ec))
		{
			std::string filename = entry.path().filename().string();
			if (filename.find(sWildcard) != std::string::npos)
			{
				asFiles.push_back(filename);
			}
		}
	}
	std::sort(asFiles.begin(), asFiles.end());
}

bool djFileExists(const std::string &sFilename)
{
	std::error_code ec;
	return fs::is_regular_file(sFilename, ec);
}

void djStripCRLF(char *buf)
{
	if (!buf) return;
	size_t len = std::strlen(buf);
	while (len > 0 && (buf[len - 1] == '\r' || buf[len - 1] == '\n'))
	{
		buf[--len] = '\0';
	}
}

void djStripCRLF(std::string &str)
{
	while (!str.empty() && (str.back() == '\r' || str.back() == '\n'))
	{
		str.pop_back();
	}
}

void djAppendPathS(std::string &sPath, std::string_view sAppend)
{
	if (sAppend.empty()) return;
	fs::path p(sPath);
	p /= sAppend;
	sPath = p.string();
}

bool djFolderExists(const std::string &sPath)
{
	std::error_code ec;
	return fs::is_directory(sPath, ec);
}

bool djEnsureFolderTreeExists(const std::string &sPath)
{
	std::error_code ec;
	return fs::create_directories(sPath, ec) || fs::is_directory(sPath, ec);
}
