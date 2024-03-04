#include "CapNetDetailStringData.h"
#include "CapNetStream.h"
#include "CapNetDetailDlg.h"


VOID CapNetDetailStringData::Init()
{
	auto res = Singleton<CapNetCore>::Instance().GetRawData(pApp_->m_packId);
	if (res)
	{
		SetData(res.ret);
	}
}

VOID CapNetDetailStringData::SetData(std::vector<BYTE>& byteVec)
{
	CapNetOutStreamW woss;
	for (auto i = byteVec.begin(); i != byteVec.end(); i++)
	{
		if (*i >= 32 && *i <= 126)
		{
			woss << CHAR(*i);
		}
		else
		{
			woss << " ";
		}
	}
	SetWindowTextW(woss.str().c_str());
}
