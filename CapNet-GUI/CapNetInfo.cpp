#include "CapNetInfo.h"
#include "CapNetStream.h"

VOID CapNetInfo::Init()
{
	InfoW(L"��ӭʹ��CapNet!------Zakiaatot@github 2024");
}

VOID CapNetInfo::InfoW(const WCHAR* msg, ...)
{
	va_list args;
	va_start(args, msg);
	CapNetOutStreamW woss;
	woss << L"֪ͨ: "
		<< CapNetOutStreamW::VFormat(msg, args);
	va_end(args);
	SetWindowTextW(woss.str().c_str());
	return VOID();
}

VOID CapNetInfo::ErrorW(const WCHAR* msg, ...)
{
	va_list args;
	va_start(args, msg);
	CapNetOutStreamW woss;
	woss << L"����: "
		<< CapNetOutStreamW::VFormat(msg, args);
	va_end(args);
	SetWindowTextW(woss.str().c_str());
	return VOID();
}

VOID CapNetInfo::SuccessW(const WCHAR* msg, ...)
{
	va_list args;
	va_start(args, msg);
	CapNetOutStreamW woss;
	woss << L"�ɹ�: "
		<< CapNetOutStreamW::VFormat(msg, args);
	va_end(args);
	SetWindowTextW(woss.str().c_str());
	return VOID();
}

VOID CapNetInfo::WarningW(const WCHAR* msg, ...)
{
	va_list args;
	va_start(args, msg);
	CapNetOutStreamW woss;
	woss << L"����: "
		<< CapNetOutStreamW::VFormat(msg, args);
	va_end(args);
	SetWindowTextW(woss.str().c_str());
	return VOID();
}
