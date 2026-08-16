#include "MT_Defines.h"
#include "Level_Main.h"
#include "GameInstance.h"
#include "Parsing_Maptool.h"
#include "UI_Test.h"

#include "Monster.h"
#include "Camera_Free.h"

TOOL_MODE g_eToolMode = TOOL_MODE::NONE;

CLevel_Main::CLevel_Main()
{
}

CLevel_Main::CLevel_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level(pDevice, pContext)
{

}

HRESULT CLevel_Main::Initialize(LEVEL _level)
{

	m_iLevel = _UINT(_level);
	g_toolType = TOOLTYPE::MAP_TOOL;

	m_pGameInstance->Build_OcTree(_float3{ 0.f, 0.f, 0.f }, 500.f, 5);

	//꼭 플레이어보다 캐릭터 컨트롤러 먼저 생성하기 
	CHECK_FAILED(Ready_Player(), E_FAIL);
	CHECK_FAILED(Ready_Object(), E_FAIL);
	CHECK_FAILED(Ready_Monster(), E_FAIL);
	CHECK_FAILED(Ready_UI(), E_FAIL);
	CHECK_FAILED(Ready_Light(), E_FAIL);

	m_pGameInstance->Set_UIManager_Active(true);
	m_pMapParser = Parsing_Maptool::Create();

#pragma region 카메라
	m_pGameInstance->CameraManager_Reset(_UINT(CAMERA::END));

	CCamera_Free::CAMFREE_DESC camDesc{};
	camDesc.vPosition = _float4(-212.933f, -18.2085f, 21.4995f, 1.f);
	camDesc.vEye = _float3(-212.933f, -18.2085f, 21.4995f);
	camDesc.vAt = _float3(0.f, 0.f, 0.f);
	camDesc.fFov = XMConvertToRadians(45.f);
	camDesc.fNear = 0.1f;
	camDesc.fFar = 500.f;
	camDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
	camDesc.fSensor = 0.08f;
	camDesc.fSpeed = 13.f;
	camDesc.fRotationSpeed = XMConvertToRadians(180.f);
	camDesc.fSmoothness = 19.f;

	m_pGameInstance->Add_Camera(m_iLevel, L"Prototype_GameObject_Camera_Free", m_iLevel, _UINT(CAMERA::FREE), nullptr, &camDesc);

	m_pGameInstance->Camera_Change(_UINT(CAMERA::FREE));
#pragma endregion

	return S_OK;
}

HRESULT CLevel_Main::Ready_UI()
{
	return S_OK;
}

HRESULT CLevel_Main::Ready_Light()
{
	LIGHT_DESC desc{};

	desc.eType = LIGHT::DIRECTIONAL;
	desc.vDiffuse = _float4{ 1.f, 1.f, 1.f, 1.f };
	desc.vAmbient = _float4{ 0.5f, 0.5f, 0.5f, 1.f };
	desc.vSpecular = _float4{ 0.8f, 0.85f, 0.9f, 1.f };
	desc.vDirection = _float4(0.f, -0.6f, 1.f, 0.f);
	//임시라서 그냥 자체 생산?
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), desc);

	SHADOW_DESC		ShadowDesc{};
	ShadowDesc.vPosition = _float4{ 0.f, 14.f, -20.f, 1.f };
	ShadowDesc.vAt = _float4{ -155.61f, 32.14f, 65.48f, 1.f };
	ShadowDesc.vOrthSize = 500;//XMConvertToRadians(45.0f);
	ShadowDesc.fNearZ = 0.1f;
	ShadowDesc.fFarZ = 500.f;
	ShadowDesc.fDepth = 500;

	if (FAILED(m_pGameInstance->Add_ShadowLight(ShadowDesc)))
		return E_FAIL;

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

_int CLevel_Main::Update_Priority(const _float fTimeDelta)
{
	m_pGameInstance->Update_Priority(fTimeDelta);
	return 0;
}

_int CLevel_Main::Update(const _float fTimeDelta)
{
	m_pGameInstance->Update_Parallel(fTimeDelta);

	if (m_pGameInstance->Get_DIKeyState(DIK_F1) & 0x80)
	{
		if (m_pGameInstance->KeyDown(DIK_F1)) 
		{
			static bool bFreeMode = true;
			bFreeMode = !bFreeMode;

			if (bFreeMode)
			{
				m_pGameInstance->Camera_Change(_UINT(CAMERA::FREE));
				Camera* pCamera = m_pGameInstance->Get_Camera(_UINT(CAMERA::PLAYER_FOCUS_00)); 
				_vector vPos = pCamera->Get_Position();

				auto pFreeCam = (CCamera_Free*)m_pGameInstance->Get_Camera(_UINT(CAMERA::FREE));
				if (pFreeCam)
				{
					_vector vPos = pCamera->Get_Position();

					vPos = XMVectorSetW(vPos, 1.f);
					pFreeCam->Set_CameraPosition(vPos);
				}
			}
			else
			{
				m_pGameInstance->Camera_Change(_UINT(CAMERA::PLAYER_FOCUS_00));
			}
		}
	}
	m_pGameInstance->Update_PhysX(fTimeDelta);
	m_pGameInstance->Update(fTimeDelta);

	return 0;
}

_int CLevel_Main::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

	return 0;
}

HRESULT CLevel_Main::Render(const _float fTimeDelta)
{

	return S_OK;
}

HRESULT CLevel_Main::Ready_Object()
{
	
	return S_OK;
}

HRESULT CLevel_Main::Ready_Player()
{
	return S_OK;
}

HRESULT CLevel_Main::Ready_Monster()
{
	return S_OK;
}

HRESULT CLevel_Main::Ready_Collider()
{
	m_pGameInstance->Add_CollisionPair(COLLISION_GROUP::PLAYER, COLLISION_GROUP::INTERACTION);
	m_pGameInstance->Add_CollisionPair(COLLISION_GROUP::PLAYER, COLLISION_GROUP::EVENT_POINT);

	return S_OK;
}

CLevel_Main* CLevel_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	CLevel_Main* pInstance = new CLevel_Main(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"CLevel_Main 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void CLevel_Main::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"CLevel_Main 원본 생성 실패", L"경고!!!", );
}

void CLevel_Main::Free()
{
	__super::Free();

	Safe_Release(m_pMapParser);
	//Safe_Release(m_pDebugDraw);
	//Safe_Release(m_pNavMeshMgr);

	m_pGameInstance->Clear_ColliderManager();
}
