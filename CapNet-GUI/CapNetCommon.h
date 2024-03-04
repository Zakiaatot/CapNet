#ifndef _CAPNET_COMMON_H_
#define _CAPNET_COMMON_H_

template <typename T>
class CapNetCommon
{
public:
	explicit CapNetCommon(T* pApp) :pApp_(pApp) {};
	virtual ~CapNetCommon() = default;

	virtual void Init() = 0;
protected:
	T* pApp_;
	CapNetCommon(const CapNetCommon&) = delete;
	CapNetCommon(const CapNetCommon&&) = delete;
	CapNetCommon& operator=(const CapNetCommon&) = delete;
	CapNetCommon& operator=(const CapNetCommon&&) = delete;
};

#endif // !_CAPNET_COMMON_H_


