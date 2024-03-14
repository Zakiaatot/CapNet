#include "CapNetGUIDlg.h"
#include <Windows.h>
#include <atlstr.h>
#include "CapNetUtils.h"


static INT AlertW(UINT option, const wchar_t* msg)
{
	return MessageBoxW(CCapNetGUIDlg::GetHwnd(), msg, L"CapNet-GUI", option | MB_APPLMODAL);
}

static INT AlertA(UINT option, const char* msg)
{
	return MessageBoxA(CCapNetGUIDlg::GetHwnd(), msg, "CapNet-GUI", option | MB_APPLMODAL);
}


void CapNetUtils::AlertErrorW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	AlertW(MB_OK | MB_ICONSTOP, buf);
}

void CapNetUtils::AlertErrorA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	AlertA(MB_OK | MB_ICONSTOP, buf);
}

void CapNetUtils::AlertWarningW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	AlertW(MB_OK | MB_ICONWARNING, buf);
}

void CapNetUtils::AlertWarningA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	AlertA(MB_OK | MB_ICONWARNING, buf);
}

void CapNetUtils::AlertInfoW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	AlertW(MB_OK | MB_ICONINFORMATION, buf);
}

void CapNetUtils::AlertInfoA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	AlertA(MB_OK | MB_ICONINFORMATION, buf);
}

int CapNetUtils::AlertQueW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	return AlertW(MB_YESNO | MB_ICONQUESTION, buf);
}

int CapNetUtils::AlertQueA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	return AlertA(MB_YESNO | MB_ICONQUESTION, buf);
}

BOOL CapNetUtils::ReleaseResource(IN UINT uResourceId, IN PCWCHAR szResourceType, IN PCWCHAR szFileName, IN PCWCHAR selfDllName, IN OUT PWCHAR errMsg)
{
	PCWCHAR msg = NULL;

	do
	{
		// 如果是 EXE 则 hMod 直接为 NULL
		HMODULE hMod = NULL;
		if (selfDllName)
			hMod = GetModuleHandle(selfDllName);

		// Find Resource
		HRSRC hRsrc = FindResource(hMod, MAKEINTRESOURCE(uResourceId), szResourceType);
		if (hRsrc == NULL)
		{
			msg = L"Find Resource Error!";
			break;
		}

		// Get Resource Size
		DWORD dwSize = SizeofResource(hMod, hRsrc);
		if (dwSize <= 0)
		{
			msg = L"Get Resource Error!";
			break;
		}

		// Load Resource
		HGLOBAL hGlobal = LoadResource(hMod, hRsrc);
		if (hGlobal == NULL)
		{
			msg = L"Load Resource Error!";
			break;
		}

		// Lock Resource
		LPVOID lpRes = LockResource(hGlobal);
		if (lpRes == NULL)
		{
			msg = L"Lock Resource Error!";
			break;
		}
		HANDLE hFile = CreateFile
		(
			szFileName,
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		if (hFile == NULL)
		{
			msg = L"Create File Error!";
			break;
		}
		DWORD dwWriten = 0;
		BOOL bRes = WriteFile(hFile, lpRes, dwSize, &dwWriten, NULL);
		CloseHandle(hFile);
		if (bRes == FALSE || dwWriten <= 0)
		{
			msg = L"Write File Error!";
			break;
		}
	} while (FALSE);

	if (msg == NULL)
	{
		return TRUE;
	}
	else
	{
		memcpy(errMsg, msg, wcslen(msg) * sizeof(WCHAR));
		return FALSE;
	}
}
