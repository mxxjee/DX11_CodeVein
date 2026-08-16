#include "AnimationTool_Define.h"
#include "Level_Main.h"

#include "GameInstance.h"
#include "Mouse.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
AnimationTool::Level_Main::Level_Main()
{
}

AnimationTool::Level_Main::Level_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level(pDevice, pContext), m_pMouse(Mouse::GetInstance())
{
}

AnimationTool::Level_Main::~Level_Main()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::Level_Main::Initialize(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	CHECK_FAILED(Ready_Player(), E_FAIL);
	CHECK_FAILED(Ready_Camera(), E_FAIL);
	CHECK_FAILED(Ready_Light(), E_FAIL);
	CHECK_FAILED(Ready_Effects(), E_FAIL);

	m_pMouse->Initialize(m_pDevice, m_pContext);
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



HRESULT Level_Main::Ready_Player()
{

	return S_OK;
}

HRESULT Level_Main::Ready_Camera()
{
	m_pGameInstance->CameraManager_Reset(_UINT(CAMERA::END));
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Free, m_iLevel, _UINT(CAMERA::FREE));
	m_pGameInstance->Camera_Change(_UINT(CAMERA::FREE));

	return S_OK;
}

HRESULT Level_Main::Ready_Light()
{
	LIGHT_DESC Desc;
	Desc.eType = LIGHT::DIRECTIONAL;
	Desc.vDiffuse = { 1.f, 1.f, 1.f, 1.f };
	Desc.vAmbient = { 1.f, 1.f, 1.f, 1.f };
	Desc.vSpecular = { 0.8f, 0.85f, 0.9f, 1.f }; //0.3으로 해놨었음 
	Desc.vDirection = { 0.f, -0.6f, 1.f, 0.f };

	//임시라서 그냥 자체 생산?
	m_pGameInstance->Add_Light(0, Desc);

	//Desc.eType = LIGHT::POINT;
	//Desc.vPosition = _float4(10.f, 5.f, 10.f, 1.f);
	//Desc.fRange = 10.f;
	//Desc.vDiffuse = _float4(1.f, 0.3f, 0.3f, 1.f);
	//Desc.vAmbient = _float4(0.3f, 0.1f, 0.1f, 1.f);
	//Desc.vSpecular = Desc.vDiffuse;

	//m_pGameInstance->Add_Light(1, Desc);

	//Desc.eType = LIGHT::POINT;
	//Desc.vPosition = _float4(10.f, 5.f, 10.f, 1.f);
	//Desc.fRange = 10.f;
	//Desc.vDiffuse = _float4(1.f, 0.3f, 0.3f, 1.f);
	//Desc.vAmbient = _float4(0.3f, 0.1f, 0.1f, 1.f);
	//Desc.vSpecular = Desc.vDiffuse;

	//m_pGameInstance->Add_Light(2, Desc);
	//GameObject* temp = m_pGameInstance->Get_GameObject(_UINT(LEVEL::MAIN), L"Layer_Player", L"Player");

	SHADOW_DESC		ShadowDesc{};
	ShadowDesc.vPosition = _float4{ 0.f, 14.f, -20.f, 1.f };
	ShadowDesc.vAt = _float4{ -155.61f, 32.14f, 65.48f, 1.f };
	ShadowDesc.vOrthSize = 500;//XMConvertToRadians(45.0f);
	ShadowDesc.fNearZ = 0.1f;
	ShadowDesc.fFarZ = 500.f;
	ShadowDesc.fDepth = 500;

	//if (FAILED(m_pGameInstance->Add_ShadowLight(ShadowDesc)))
	//	return E_FAIL;

	SHADOW_DESC		ShadowDesc1{};
	ShadowDesc1.vPosition = _float4{ 0.f, 1.f, -1.f, 1.f }; //높이서 내려다봄
	ShadowDesc1.vAt = _float4{ 0.f, 0.f, 0.f, 1.f };
	ShadowDesc1.vOrthSize = 40.f;
	ShadowDesc1.fNearZ = 0.1f;
	ShadowDesc1.fFarZ = 500.f;
	ShadowDesc1.fDepth = 200.f;

	const _int NumCascade = 4;
	_float bias[NumCascade] = { 0.001f, 0.002f, 0.003f, 0.005f };
	if (FAILED(m_pGameInstance->Add_ShadowCascadeLight(NumCascade, ShadowDesc1, bias)))
		return E_FAIL;

	return S_OK;
}

HRESULT Level_Main::Ready_Effects()
{
	m_pParsing_VFX = VFX_Parsing::Create();

	// JSON 파일 목록
	static vector<_string> vecEffectFiles;
	static _bool bScan = { false };
	if (bScan == false)
	{
		namespace fs = std::filesystem;
		fs::path effectDir("../../DataFiles/Effects");
		if (fs::exists(effectDir))
		{
			for (auto& effect : fs::directory_iterator(effectDir))
			{
				if (effect.path().extension() == ".json")
					vecEffectFiles.push_back(effect.path().stem().string());	// ABC.json => ABC로 바꿔줌
			}
		}
		bScan = true;
	}

	// 컨테이너에 하나도 없다면 이벤트 발생 안함
		if (vecEffectFiles.size() != 0)
	{
		for (_uint i = 0; i < vecEffectFiles.size(); i++)
		{
			LOADEVENT event;
			event.eToolType = TOOLTYPE::VFX_TOOL;
			event.m_Path = "../../DataFiles/Effects/" + vecEffectFiles[i] + ".json";
			m_pGameInstance->Publish<LOADEVENT>(event);
		}
	}

	return S_OK;
}


//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int AnimationTool::Level_Main::Update_Priority(const _float fTimeDelta)
{
	m_pGameInstance->Update_Priority(fTimeDelta);

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int AnimationTool::Level_Main::Update(const _float fTimeDelta)
{
	GameObject* temp = {};
	if (m_pGameInstance->Get_DIKeyState(DIK_Q))
	{
		m_btrue = !m_btrue;
	}
	if (m_btrue)
	{
		if (m_pMouse->Picking_Object_GameObject_Pixel(temp))
		{
			cout << temp << endl;
		}
	}

	m_pGameInstance->Update_Parallel(fTimeDelta);
	m_pGameInstance->Update(fTimeDelta);
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int AnimationTool::Level_Main::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::Level_Main::Render(const _float fTimeDelta)
{

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("메인레벨입니다."));
#endif

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Main* AnimationTool::Level_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Main* pInstance = new Level_Main(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_Main 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void AnimationTool::Level_Main::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Main 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void AnimationTool::Level_Main::Free()
{
	Mouse::DestroyInstance();
	__super::Free();

	Safe_Release(m_pParsing_VFX);
}
/******************************************************* 객체 반환 함수 *******************************************************/
