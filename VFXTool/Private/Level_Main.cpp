#include "VFXTool_Define.h"
#include "Level_Main.h"

#include "ImguiManager.h"
#include "Window_VFXEditor.h"
#include "BasicParticle.h"
#include "ParticleSystem.h"
#include "Decal_Blood.h"
#include "GameInstance.h"

#include "VFX_Parsing.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
VFXTool::Level_Main::Level_Main()
{
}

VFXTool::Level_Main::Level_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level(pDevice, pContext)
{
	m_pImguiManager = ImguiManager::GetInstance();
	m_pParser = VFX_Parsing::Create();
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT VFXTool::Level_Main::Initialize(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	CHECK_FAILED(Ready_Player(), E_FAIL);
	CHECK_FAILED(Ready_Camera(), E_FAIL);
	CHECK_FAILED(Ready_Light(), E_FAIL);
	CHECK_FAILED(Ready_ImGui(), E_FAIL);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int VFXTool::Level_Main::Update_Priority(const _float fTimeDelta)
{
	m_pGameInstance->Update_Priority(fTimeDelta);

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int VFXTool::Level_Main::Update(const _float fTimeDelta)
{
	m_pGameInstance->Update(fTimeDelta);
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int VFXTool::Level_Main::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT VFXTool::Level_Main::Render(const _float fTimeDelta)
{

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/

HRESULT Level_Main::Ready_Player()
{
	_wstring layername = L"Layer_Player";

	CHECK_FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::MAIN), (L"Prototype_GameObject_Player"), _UINT(LEVEL::MAIN), layername), E_FAIL);

	return S_OK;
}

HRESULT Level_Main::Ready_Camera()
{
	m_pGameInstance->CameraManager_Reset(_UINT(CAMERA::END));
	CHECK_FAILED(m_pGameInstance->Add_Camera(m_iLevel, L"Prototype_GameObject_Camera_Free", m_iLevel, _UINT(CAMERA::FREE)), E_FAIL);
	m_pGameInstance->Camera_Change(_UINT(CAMERA::FREE));

	GameObject::GAMEOBJECT_DESC Desc;
	Desc.wstrName = L"Terrain";
	CHECK_FAILED(m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, L"Prototype_GameObject_Terrain", m_iLevel, L"Layer_Terrain", nullptr, &Desc), E_FAIL);

	return S_OK;
}

HRESULT Level_Main::Ready_Light()
{
	LIGHT_DESC desc{};

	desc.eType = LIGHT::DIRECTIONAL;
	desc.vDirection = _float4(-1.f, -1.f, -1.f, 0.f);
	desc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	desc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	desc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	m_pGameInstance->Add_Light(0, desc);

	//SHADOW_DESC		ShadowDesc{};
	//ShadowDesc.vPosition = _float4(10.f, 10.f, 0.f, 1.f);
	//ShadowDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	//ShadowDesc.fFovy = XMConvertToRadians(45.0f);
	//ShadowDesc.fNearZ = 0.1f;
	//ShadowDesc.fFarZ = 1500.f;


	//if (FAILED(m_pGameInstance->Add_ShadowLight(ShadowDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT Level_Main::Ready_ImGui()
{
	Window_VFXEditor* pWindowEditor = dynamic_cast<Window_VFXEditor*>(m_pImguiManager->Find_Window("VFX_Editor"));
	if (pWindowEditor == nullptr)
		return E_FAIL;

	pWindowEditor->Set_Parser(m_pParser);

	return S_OK;
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Main* VFXTool::Level_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Main* pInstance = new Level_Main(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_Main 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void VFXTool::Level_Main::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Main 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void VFXTool::Level_Main::Free()
{
	__super::Free();

	m_pImguiManager->DestroyInstance();
	Safe_Release(m_pParser);
}
/******************************************************* 객체 반환 함수 *******************************************************/
