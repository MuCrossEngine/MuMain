#pragma once
class CBDrawInterface
{
	CBDrawInterface();
	virtual ~CBDrawInterface();
public:
	static CBDrawInterface* Instance();
	void RenderFrame();
};
#define gCBDrawInterface			(CBDrawInterface::Instance())