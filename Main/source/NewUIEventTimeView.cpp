#include "stdafx.h"
#include "NewUISystem.h"
#include "NewUIEventTimeView.h"

SEASON3B::CNewUIEventTimeView::CNewUIEventTimeView()
{
	m_pNewUIMng = NULL;
	m_Pos.x = 0;
	m_Pos.y = 0;
	this->Init();
}

SEASON3B::CNewUIEventTimeView::~CNewUIEventTimeView()
{
	Release();
}

bool SEASON3B::CNewUIEventTimeView::Create(CNewUIManager* pNewUIMng, float x, float y)
{
	bool Success = false;

	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_EVENT_TIME, this);

		this->LoadImages();

		this->SetPos(x, y);

		this->SetInfo();

		this->Show(false);

		Success = true;
	}
	return Success;
}

void SEASON3B::CNewUIEventTimeView::Release()
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);

		this->UnloadImages();
	}
}

void SEASON3B::CNewUIEventTimeView::Init()
{

	m_EventIndex = 0;
	m_EventTotal[0] = 0;
	m_EventTotal[1] = 0;
	m_EventTotal[2] = 0;
	m_iViewCount = 10;
	m_EventTimeTickCount = 0;
}

void SEASON3B::CNewUIEventTimeView::SetInfo()
{
}

void SEASON3B::CNewUIEventTimeView::SetPos(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void SEASON3B::CNewUIEventTimeView::LoadImages()
{

	LoadBitmap("Interface\\HUD\\top_back_3.tga", IMAGE_TOP_BACK3, GL_LINEAR);

}

void SEASON3B::CNewUIEventTimeView::UnloadImages()
{
	DeleteBitmap(IMAGE_TOP_BACK3);
}

bool SEASON3B::CNewUIEventTimeView::UpdateKeyEvent()
{
	if (IsVisible() == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(INTERFACE_EVENT_TIME);
			return false;
		}
	}
	return true;
}

bool SEASON3B::CNewUIEventTimeView::UpdateMouseEvent()
{
	if (SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, 214.f, 230.f))
	{
		if (SEASON3B::CheckMouseIn(m_Pos.x + 192.f, m_Pos.y + 3.5f, 19.f, 19.f) && SEASON3B::IsRelease(VK_LBUTTON))
		{
			g_pNewUISystem->Hide(INTERFACE_EVENT_TIME);
			return false;
		}
	}
	return !SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, 214.f, 230.f);
}

bool SEASON3B::CNewUIEventTimeView::Render()
{
	EnableAlphaTest(true);

	glColor4f(1.f, 1.f, 1.f, 1.f);

	RenderFrame();

	RenderTexte();

	DisableAlphaBlend();

	return true;
}

bool SEASON3B::CNewUIEventTimeView::Update()
{
	return true;
}

float SEASON3B::CNewUIEventTimeView::GetLayerDepth()
{
	return 10.0f;
}

void SEASON3B::CNewUIEventTimeView::OpenningProcess()
{
}

void SEASON3B::CNewUIEventTimeView::ClosingProcess()
{
}

void SEASON3B::CNewUIEventTimeView::RenderFrame()
{
	float x = m_Pos.x;
	float y = m_Pos.y;

	RenderImageF(IMAGE_TOP_BACK3, x, y, 214.f, 230.f, 0.0, 0.0, 854.0, 917.0); //-- background
}

void SEASON3B::CNewUIEventTimeView::RenderTexte()
{
}
