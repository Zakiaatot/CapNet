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
		BOOL success = FALSE;
		std::string msg;
		T ret{};
		operator bool() { return success == TRUE; };
		bool operator!() { return success == FALSE; };
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
	typedef VOID(*LISTEN_END_CALLBACK_FUNC)(std::wstring& msg);
	typedef struct _TreeNode
	{
		_TreeNode* parent;
		_TreeNode* firstBrother;
		_TreeNode* firstChild;
		WCHAR* text;
	} PacDetailTreeNode, * PPacDetailTreeNode, * PacDetailTree, * PacDetailTreeRoot;

	StatusVoid Init(); // 初始化
	Status<std::vector<std::pair<int, std::string>>> FindAllDev(); // 获取所有网卡
	StatusVoid FreeAllDev(); // 释放所有网卡
	StatusVoid BeginListen // 开始监听
	(
		INT devId,
		std::wstring  rule,
		LISTEN_CALLBACK_FUNC pCallback,
		LISTEN_END_CALLBACK_FUNC pEndCallback
	);
	StatusVoid EndListen(); // 结束监听
	StatusVoid SetFilter(std::wstring rule); // 设置过滤器
	Status<std::vector<BYTE>> GetRawData(UINT pacId); // 获取原始包数据
	Status<PacDetailTree> GenDetailTree(UINT pacId); // 生成报文详情树
	StatusVoid DelDetailTree(PacDetailTree tree); // 销毁报文详情树
	StatusVoid IsSaveable(); // 是否可保存
	StatusVoid SaveFile(PCWCHAR filePath); // 保存文件
	StatusVoid LoadFile
	(
		CapNetCore::LISTEN_CALLBACK_FUNC pCallback,
		CapNetCore::LISTEN_END_CALLBACK_FUNC pEndCallback,
		PCWCHAR filePath
	); // 加载文件
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
