
#include "stdafx.h"
#include "NewUIManager.h"
#include "./Utilities/Log/ErrorReport.h"


using namespace SEASON3B;

SEASON3B::CNewUIManager::CNewUIManager() 
{
	m_pActiveMouseUIObj = NULL;
	m_pActiveKeyUIObj = NULL;
#ifdef PBG_MOD_STAMINA_UI
	m_nShowUICnt =0;
#endif //PBG_MOD_STAMINA_UI
}

SEASON3B::CNewUIManager::~CNewUIManager() 
{ 
	RemoveAllUIObjs(); 
}

void SEASON3B::CNewUIManager::AddUIObj(DWORD dwKey, CNewUIObj* pUIObj)
{
	type_map_uibase::iterator mi = m_mapUI.find(dwKey);
	if(mi == m_mapUI.end())
	{
		m_vecUI.push_back(pUIObj);
		m_mapUI.insert( type_map_uibase::value_type(dwKey, pUIObj) );
	}
}

void SEASON3B::CNewUIManager::RemoveUIObj(DWORD dwKey)
{
	type_map_uibase::iterator mi = m_mapUI.find(dwKey);
	if(mi != m_mapUI.end())
	{
		type_vector_uibase::iterator vi = std::find(m_vecUI.begin(), m_vecUI.end(), (*mi).second);
		if(vi != m_vecUI.end())
		{
			m_vecUI.erase(vi);
		}
		m_mapUI.erase(mi);
	}
}

void SEASON3B::CNewUIManager::RemoveUIObj(CNewUIObj* pUIObj)
{
	type_map_uibase::iterator mi = m_mapUI.begin();
	for(; mi != m_mapUI.end(); mi++)
	{
		if((*mi).second == pUIObj)
		{
			m_mapUI.erase(mi);
			break;
		}
	}
		
	type_vector_uibase::iterator vi = std::find(m_vecUI.begin(), m_vecUI.end(), pUIObj);
	if(vi != m_vecUI.end())
	{
		m_vecUI.erase(vi);
	}
}

void SEASON3B::CNewUIManager::RemoveAllUIObjs()
{
#if defined(_DEBUG)
	
	{
		unsigned int uiUIManageCNT = m_mapUI.size();
		
		type_map_uibase::iterator mi = m_mapUI.begin();
		for( ; mi != m_mapUI.end(); ++mi )
		{
			DWORD dwKey = (*mi).first;
			CNewUIObj* pUIObj = (*mi).second;
			if( pUIObj != NULL )
			{
				__TraceF(TEXT("UIKEY(%d) : mapUI \n"), uiUIManageCNT, dwKey);
			}
		}
		
		type_vector_uibase::iterator vi = m_vecUI.begin();
		for( ; vi < m_vecUI.end(); ++vi )
		{
			CNewUIObj* pUIObj = (*vi);
			if( pUIObj != NULL )
			{
				__TraceF(TEXT("vecUI \n"), uiUIManageCNT);
			}
		}
	}

#endif // defined(_DEBUG)
	m_vecUI.clear();
	m_mapUI.clear();
}

CNewUIObj* SEASON3B::CNewUIManager::FindUIObj(DWORD dwKey)
{
	type_map_uibase::iterator mi = m_mapUI.find(dwKey);
	if(mi != m_mapUI.end())
		return (*mi).second;
	return NULL;
}

bool SEASON3B::CNewUIManager::UpdateMouseEvent()
{
	m_pActiveMouseUIObj = NULL;

	std::sort(m_vecUI.begin(), m_vecUI.end(), CompareLayerDepthReverse);
	
	type_vector_uibase::iterator vi = m_vecUI.begin();
	vi = m_vecUI.begin();
	for(; vi != m_vecUI.end(); vi++)
	{
		if((*vi)->IsVisible()) 
		{

			CNewUIObj *obj_backup = (*vi);
			bool bResult = (*vi)->UpdateMouseEvent();
			
			type_vector_uibase::iterator vi2 = std::find(m_vecUI.begin(), m_vecUI.end(), obj_backup);
			if( vi2 != m_vecUI.end() )
			{
				vi = vi2;
			}
			else
			{
				break;
			}

			if( bResult == false )
			{
				m_pActiveMouseUIObj = *vi;
				return false;
			}

		}
	}

	return true;
}

bool SEASON3B::CNewUIManager::UpdateKeyEvent()
{
	m_pActiveKeyUIObj = NULL;
	std::sort(m_vecUI.begin(), m_vecUI.end(), CompareKeyEventOrder);
	
	type_vector_uibase::iterator vi = m_vecUI.begin();
	for(; vi != m_vecUI.end(); vi++)
	{
		HWND hRelatedWnd = (*vi)->GetRelatedWnd();
		if(NULL == hRelatedWnd)
		{
			hRelatedWnd = gwinhandle->GethWnd();
		}

		HWND hWnd = GetFocus();

		if((*vi)->IsEnabled() && hWnd == hRelatedWnd)
		{
			if(false == (*vi)->UpdateKeyEvent())
			{
				m_pActiveKeyUIObj = (*vi);
				return false;		//. stop calling UpdateKeyEvent functions
			}
		}
	}
	return true;
}

bool SEASON3B::CNewUIManager::Update()
{
	std::sort(m_vecUI.begin(),m_vecUI.end(), CompareLayerDepth);
	
	type_vector_uibase::iterator vi = m_vecUI.begin();
	for(; vi != m_vecUI.end(); vi++)
	{
		if((*vi)->IsEnabled()) 
		{
			if(false == (*vi)->Update())
			{
				return false;		//. stop calling Update functions
			}
		}
	}

	return true;
}

bool SEASON3B::CNewUIManager::Render()
{
	std::sort(m_vecUI.begin(),m_vecUI.end(), CompareLayerDepth);
	
	type_vector_uibase::iterator vi = m_vecUI.begin();

	glAlphaFunc(GL_GREATER, 0.0000000);

	for(; vi != m_vecUI.end(); vi++)
	{
		if((*vi)->IsVisible()) 
		{
			(*vi)->Render();
		}
	}

	glAlphaFunc(GL_GREATER, 0.2500000);

	return true;
}

CNewUIObj* SEASON3B::CNewUIManager::GetActiveMouseUIObj()
{ 
	return m_pActiveMouseUIObj; 
}

CNewUIObj* SEASON3B::CNewUIManager::GetActiveKeyUIObj()
{ 
	return m_pActiveKeyUIObj; 
}

void SEASON3B::CNewUIManager::ResetActiveUIObj()
{
	m_pActiveMouseUIObj = NULL;
	m_pActiveKeyUIObj = NULL;
}

bool SEASON3B::CNewUIManager::IsInterfaceVisible(DWORD dwKey)
{
	CNewUIObj* pObj = FindUIObj(dwKey);
	if(NULL == pObj)
	{
		return false;
	}
	return pObj->IsVisible();
}

bool SEASON3B::CNewUIManager::IsInterfaceEnabled(DWORD dwKey)
{
	CNewUIObj* pObj = FindUIObj(dwKey);
	if(NULL == pObj)
		return false;
	return pObj->IsEnabled();
}

void SEASON3B::CNewUIManager::ShowInterface(DWORD dwKey, bool bShow/* = true*/)
{
	CNewUIObj* pObj = FindUIObj(dwKey);
	if(NULL != pObj)
		pObj->Show(bShow);
}

void SEASON3B::CNewUIManager::EnableInterface(DWORD dwKey, bool bEnable/* = true*/)
{
	CNewUIObj* pObj = FindUIObj(dwKey);
	if(NULL != pObj)
		pObj->Enable(bEnable);
}

void SEASON3B::CNewUIManager::ShowAllInterfaces(bool bShow/* = true*/)
{
	type_map_uibase::iterator mi = m_mapUI.begin();
	for(; mi != m_mapUI.end(); mi++)
		(*mi).second->Show(bShow);
}

void SEASON3B::CNewUIManager::EnableAllInterfaces(bool bEnable/* = true*/)
{
	type_map_uibase::iterator mi = m_mapUI.begin();
	for(; mi != m_mapUI.end(); mi++)
		(*mi).second->Show(bEnable);
}

void SEASON3B::CNewUIManager::UpdateInterface(DWORD dwKey, double WindowsX, double WindowsY)
{
	CNewUIObj* pObj = FindUIObj(dwKey);
	if (NULL != pObj)
		pObj->EventOrderWindows(WindowsX, WindowsY);
}

bool SEASON3B::CNewUIManager::CompareLayerDepth(INewUIBase* pObj1,INewUIBase* pObj2)
{ 
	return pObj1->GetLayerDepth() < pObj2->GetLayerDepth(); 
}

bool SEASON3B::CNewUIManager::CompareLayerDepthReverse(INewUIBase* pObj1,INewUIBase* pObj2)
{ 
	return pObj1->GetLayerDepth() > pObj2->GetLayerDepth(); 
}

bool SEASON3B::CNewUIManager::CompareKeyEventOrder(INewUIBase* pObj1,INewUIBase* pObj2)
{
	return pObj1->GetKeyEventOrder() > pObj2->GetKeyEventOrder();
}

#ifdef PBG_MOD_STAMINA_UI
int SEASON3B::CNewUIManager::GetShowUICnt()
{
	int m_nShowUICnt=0;
	// 일부 특정 인터페이스가 몇개 열려있는지
	for(int i=INTERFACE_PARTY; i<INTERFACE_CHARACTER+1; ++i)
	{
		if(IsInterfaceVisible(i))
			m_nShowUICnt++;
	}
	return m_nShowUICnt;
}
#endif //PBG_MOD_STAMINA_UI
