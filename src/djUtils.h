//---------------------------------------------------------------------------
// djUtils.h
//
// Copyright (C) 2002-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#ifndef _DJUTILS_H_
#define _DJUTILS_H_

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

#ifndef UINT
typedef unsigned int UINT;
#endif

//! Deleters inline modernos e seguros
template <typename T>
inline void djDEL(T*& p) noexcept
{
	delete p;
	p = nullptr;
}

template <typename T>
inline void djDELV(T*& p) noexcept
{
	delete[] p;
	p = nullptr;
}

//! Min / Max / Abs helpers
template <typename T>
constexpr const T& djMIN(const T& a, const T& b) { return (a < b) ? a : b; }

template <typename T>
constexpr const T& djMAX(const T& a, const T& b) { return (a > b) ? a : b; }

template <typename T>
constexpr T djABS(const T& a) { return (a < 0) ? -a : a; }

//! Write a thread-safe debug message
void djLog(const char *szFormat, ...);

//! Assertion helper
void djAssert(bool bExpression, const char *szFile, int nLine, const char *szExpression);

#define djASSERT(Expression) djAssert(static_cast<bool>(Expression), __FILE__, __LINE__, #Expression)

#ifdef _DEBUG
#define djASSERTD(Expression) djASSERT(Expression)
#else
#define djASSERTD(Expression) ((void)0)
#endif

int djStrCaseCmp(std::string_view s1, std::string_view s2);

void djFindFiles(const std::string &sDirectory, const std::string &sWildcard, std::vector<std::string> &asFiles);
bool djFileExists(const std::string &sFilename);

//! Strip newline character from end of string
void djStripCRLF(char *buf);
void djStripCRLF(std::string &str);

void djAppendPathS(std::string &sPath, std::string_view sAppend);

bool djFolderExists(const std::string &sPath);
bool djEnsureFolderTreeExists(const std::string &sPath);

#endif
