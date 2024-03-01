#ifndef  _CAPNET_CORE_H_
#define _CAPNET_CORE_H_

#ifdef DLLEXPORT
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif // DLLEXPORT

#include <string>
#include <vector>
#include <Windows.h>


template<typename T>
class Singleton
{
public:
	static T& Instance()
	{
		static T instance;
		return instance;
	}
private:
	Singleton() = default;
	~Singleton() = default;
	Singleton(const Singleton&) = delete;
	Singleton(const Singleton&&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton& operator=(const Singleton&&) = delete;
};

class DLL CapNetCore
{
	friend class Singleton<CapNetCore>;
public:
	template <typename T>
	struct Status
	{
		BOOL success;
		std::string msg;
		T ret;
		operator bool() { return success == TRUE; };
		bool operator!() { return !bool(this); };
		static Status Ok(T res = 0) { return { TRUE,"",res }; };
		static Status Err(const char* msg, ...) {
			CHAR buf[128];
			va_list args;
			va_start(args, msg);
			vsprintf_s(buf, msg, args);
			va_end(args);
			return { FALSE,buf,T() };
		};
	};
	using StatusVoid = typename Status<VOID*>;
	struct Pack
	{
		UINT no;
		UINT length;
		std::wstring time;
		std::wstring source;
		std::wstring dest;
		std::wstring protocol;
		std::wstring info;
	};
	typedef VOID(*LISTEN_CALLBACK_FUNC)(CapNetCore::Pack& pack);

	StatusVoid Init(); // 初始化
	Status<std::vector<std::pair<int, std::string>>> FindAllDev(); // 获取所有网卡
	StatusVoid FreeAllDev(); // 释放所有网卡
	StatusVoid BeginListen // 开始监听
	(
		INT devId,
		const WCHAR* type,
		const WCHAR* ip,
		LISTEN_CALLBACK_FUNC pCallback
	);
	StatusVoid EndListen(); // 结束监听
private:
	CapNetCore() = default;
	~CapNetCore() = default;
	CapNetCore(const CapNetCore&) = delete;
	CapNetCore(const CapNetCore&&) = delete;
	CapNetCore& operator=(const CapNetCore&) = delete;
	CapNetCore& operator=(const CapNetCore&&) = delete;

	StatusVoid LoadNpcapDlls();

};


#endif // ! _CAPNET_CORE_H_
