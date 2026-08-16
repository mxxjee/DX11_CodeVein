#include "UITool_Define.h"
#include "Level_UIToolLoad.h"

#include "GameInstance.h"
#include "UIToolLoader.h"

#include "Level_UIMain.h"




//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
UITool::Level_UIToolLoad::Level_UIToolLoad()
{
}

UITool::Level_UIToolLoad::Level_UIToolLoad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level(pDevice, pContext)
{
}

UITool::Level_UIToolLoad::~Level_UIToolLoad()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT UITool::Level_UIToolLoad::Initialize(LEVEL _level)
{
	m_eCreateLevel = _level;
	m_iCreateLevel = _uint(_level);
	m_pLoader = UIToolLoader::Create(m_pDevice, m_pContext, _level);
	CHECK_NULLPTR(m_pLoader);

	m_pGameInstance->Set_UIManager_Active(false);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int UITool::Level_UIToolLoad::Update_Priority(const _float fTimeDelta)
{
	m_pGameInstance->Update_Priority(fTimeDelta);



	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int UITool::Level_UIToolLoad::Update(const _float fTimeDelta)
{
	m_pGameInstance->Update(fTimeDelta);

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int UITool::Level_UIToolLoad::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

	switch (m_eCreateLevel)
	{

	case LEVEL::MAIN:
		if (m_pLoader->Is_LoadComplete())
		{
			m_pGameInstance->Add_Level(_UINT(LEVEL::MAIN), Level_UIMain::Create(m_pDevice, m_pContext, m_eCreateLevel));
			m_pGameInstance->Change_Level(_UINT(LEVEL::MAIN));
		}
		break;

	}

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT UITool::Level_UIToolLoad::Render(const _float fTimeDelta)
{

	wchar_t szTitle[256];
	swprintf_s(szTitle, L"로딩씬입니다.");

	SetWindowText(g_hWnd, szTitle);


	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_UIToolLoad* UITool::Level_UIToolLoad::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_UIToolLoad* pInstance = new Level_UIToolLoad(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_UIToolLoad 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void UITool::Level_UIToolLoad::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_UIToolLoad 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void UITool::Level_UIToolLoad::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
/******************************************************* 객체 반환 함수 *******************************************************/
