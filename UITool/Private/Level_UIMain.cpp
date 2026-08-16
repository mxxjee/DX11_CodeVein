#include "UITool_Define.h"
#include "Level_UIMain.h"
#include "GameInstance.h"
#include "UIObject.h"
#include "UITool_Macro.h"
#include "UITool_Macro_Component.h"
#include "Sample_Pars_UItool.h"
#include "CopyManager.h"
#include "TestModel.h"
#include "UITool_Macro_GameObject.h"



//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
UITool::Level_UIMain::Level_UIMain()
{
}

UITool::Level_UIMain::Level_UIMain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level(pDevice, pContext)
{
}

UITool::Level_UIMain::~Level_UIMain()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT UITool::Level_UIMain::Initialize(LEVEL _level)
{
	m_iLevel = _UINT(_level);
	
	
#pragma region 카메라
	m_pGameInstance->CameraManager_Reset(_UINT(CAMERA::END));
	m_pGameInstance->Add_Camera(_UINT(LEVEL::STATIC), Proto_GameObject_Camera_Free, m_iLevel, _UINT(CAMERA::FREE));
	m_pGameInstance->Camera_Change(_UINT(CAMERA::FREE));
#pragma endregion


	_wstring layer = Layer_UIs;

	m_pGameInstance->Set_UIManager_Active(true);

	//가이드사진 불러오기
	UIObject::UIOBJECT_DESC UIDesc;
	UIDesc.fCX = WINCX;
	UIDesc.fCY = WINCY;
	UIDesc.fX = WINCX / 2.f;
	UIDesc.fY = WINCY / 2.f;
	UIDesc.iShaderNumber = 4;
	UIDesc.wstrName = L"Back";
	UIDesc.fAlpha = 0.3f;


	
	GameObject* pOut = nullptr;
	

	//if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(0, L"Prototype_UIObject", m_iLevel, layer, &pOut, &UIDesc)))
	//	return E_FAIL;


	if (pOut)
	{
		pGuideUI = dynamic_cast<UIObject*>(pOut);
		UI_Image::UIIMAGEDESC Desc;
		Desc.pOwner = pGuideUI;
		Desc.TextureKey = "Guide/LevelUp_Guide";


		if (FAILED(pGuideUI->Add_NewRenderComponent(0, Proto_UIImage, Proto_UIImage, &Desc)))
			return E_FAIL;

		pGuideUI->Set_Lock(true);
	}
	
	m_pParsing_UI = Sample_Pars_UItool::Create();
	CopyManager::GetInstance()->Set_Parser(dynamic_cast<Sample_Pars_UItool*>(m_pParsing_UI));

	CHECK_FAILED(Ready_Light(), E_FAIL);

#pragma region 모델소환
	TestModel::TestModelDesc Desc;
	Desc.modelName = L"NPC_Murasame";

	pOut = nullptr;

	CHECK_FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::STATIC), Proto_GameObject(L"TestModel"), m_iLevel, L"Model_Layer", &pOut, &Desc),E_FAIL);
	if (pOut)
		int A = 10;

#pragma endregion
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int UITool::Level_UIMain::Update_Priority(const _float fTimeDelta)
{

	m_pGameInstance->Update_Priority(fTimeDelta);

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/

HRESULT Level_UIMain::Ready_Light()
{
	LIGHT_DESC desc{};

	desc.eType = LIGHT::DIRECTIONAL;
	desc.vDirection = _float4(1.f, 1.f, 1.f, 0.f);
	desc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	desc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	desc.vSpecular = _float4(0.3f, 0.3f, 0.3f, 1.f);

	m_pGameInstance->Add_Light(0, desc);

	return S_OK;
}


//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int UITool::Level_UIMain::Update(const _float fTimeDelta)
{
	m_pGameInstance->Update(fTimeDelta);

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int UITool::Level_UIMain::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT UITool::Level_UIMain::Render(const _float fTimeDelta)
{

	wchar_t szTitle[256];
	swprintf_s(szTitle, L"UITool 씬입니다.");

	SetWindowText(g_hWnd, szTitle);
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/


//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_UIMain* UITool::Level_UIMain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_UIMain* pInstance = new Level_UIMain(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_UIMain 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void UITool::Level_UIMain::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_UIMain 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void UITool::Level_UIMain::Free()
{
	Safe_Release(m_pParsing_UI);
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
