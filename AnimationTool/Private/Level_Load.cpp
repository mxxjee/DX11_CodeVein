#include "AnimationTool_Define.h"
#include "Level_Load.h"

#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_Main.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
AnimationTool::Level_Load::Level_Load()
{
}

AnimationTool::Level_Load::Level_Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level(pDevice, pContext)
{
}

AnimationTool::Level_Load::~Level_Load()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::Level_Load::Initialize(LEVEL _level)
{
	m_eCreateLevel = _level;
	m_iCreateLevel = _uint(_level);
	m_pLoader = Loader::Create(m_pDevice, m_pContext, _level);
	CHECK_NULLPTR(m_pLoader);

	m_pGameInstance->Set_UIManager_Active(false);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int AnimationTool::Level_Load::Update_Priority(const _float fTimeDelta)
{
	m_pGameInstance->Update_Priority(fTimeDelta);



	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int AnimationTool::Level_Load::Update(const _float fTimeDelta)
{
	m_pGameInstance->Update(fTimeDelta);

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int AnimationTool::Level_Load::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

	switch (m_eCreateLevel)
	{
	case LEVEL::LOGO:
		if (m_pLoader->Is_LoadComplete())
		{
			m_pGameInstance->Add_Level(_UINT(LEVEL::LOGO), Level_Logo::Create(m_pDevice, m_pContext, m_eCreateLevel));
			m_pGameInstance->Change_Level(_UINT(LEVEL::LOGO));
		}
		break;
	case LEVEL::MAIN:
		if (m_pLoader->Is_LoadComplete())
		{
			m_pGameInstance->Add_Level(_UINT(LEVEL::MAIN), Level_Main::Create(m_pDevice, m_pContext, m_eCreateLevel));
			m_pGameInstance->Change_Level(_UINT(LEVEL::MAIN));
		}
		break;
	}

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::Level_Load::Render(const _float fTimeDelta)
{



	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Load* AnimationTool::Level_Load::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Load* pInstance = new Level_Load(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_Load 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void AnimationTool::Level_Load::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Load 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void AnimationTool::Level_Load::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
/******************************************************* 객체 반환 함수 *******************************************************/
