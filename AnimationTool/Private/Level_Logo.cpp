 #include "AnimationTool_Define.h"
#include "Level_Logo.h"
#include "Level_Load.h"
#include "GameInstance.h"
#include "Loader.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
AnimationTool::Level_Logo::Level_Logo()
{
}

AnimationTool::Level_Logo::Level_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level(pDevice, pContext)
{
}

AnimationTool::Level_Logo::~Level_Logo()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::Level_Logo::Initialize(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	CHECK_FAILED(Ready_UI(), E_FAIL);

	m_pGameInstance->Set_UIManager_Active(true);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// UI 준비 ////////////////////////////////////////////////////////
HRESULT AnimationTool::Level_Logo::Ready_UI()
{
	//_wstring layer = Layer_UIs;

	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_UIObject(L"Test"), m_iLevel, layer);
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_UIObject(L"Test"), m_iLevel, layer);
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_UIObject(L"Test"), m_iLevel, layer);

	return S_OK;
}
/******************************************************* UI 준비 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int AnimationTool::Level_Logo::Update_Priority(const _float fTimeDelta)
{
	m_pGameInstance->Update_Priority(fTimeDelta);

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int AnimationTool::Level_Logo::Update(const _float fTimeDelta)
{
	m_pGameInstance->Update(fTimeDelta);
	


	if (Loader::m_bStaticComplete == true)
	{
		m_pGameInstance->Add_Level(CAST(_uint)(LEVEL::LOADING), Level_Load::Create(m_pDevice, m_pContext, LEVEL::MAIN));

		if (FAILED(m_pGameInstance->Change_Level(CAST(_uint)(LEVEL::LOADING))))
			return 0;
	}

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int AnimationTool::Level_Logo::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::Level_Logo::Render(const _float fTimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Logo* AnimationTool::Level_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Logo* pInstance = new Level_Logo(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_Logo 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void AnimationTool::Level_Logo::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Logo 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void AnimationTool::Level_Logo::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
