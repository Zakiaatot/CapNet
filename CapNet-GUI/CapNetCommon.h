#ifndef _CAPNET_COMMON_H_
#define _CAPNET_COMMON_H_

class CCapNetGUIDlg;

class CapNetCommon
{
public:
	explicit CapNetCommon(CCapNetGUIDlg* pApp) :pApp_(pApp) {};
	virtual ~CapNetCommon() = default;

	virtual void Init() = 0;
protected:
	CCapNetGUIDlg* pApp_;
	CapNetCommon(const CapNetCommon&) = delete;
	CapNetCommon(const CapNetCommon&&) = delete;
	CapNetCommon& operator=(const CapNetCommon&) = delete;
	CapNetCommon& operator=(const CapNetCommon&&) = delete;
};

#endif // !_CAPNET_COMMON_H_


