#include "Client_Define.h"
#include "MinimapRenderComponent.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"
#include "RenderTargetManager.h"





Client::MinimapRenderComponent::MinimapRenderComponent()
{
}

Client::MinimapRenderComponent::MinimapRenderComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :Component(pDevice,pContext)
{
}

Client::MinimapRenderComponent::MinimapRenderComponent(const MinimapRenderComponent& original)
    :Component(original), m_pMinimapManager(original.m_pMinimapManager)
{
}

Client::MinimapRenderComponent::~MinimapRenderComponent()
{
}

HRESULT Client::MinimapRenderComponent::Initialize_Prototype()
{
	CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);
	m_pMinimapManager = MinimapManager::GetInstance();

	return S_OK;

}

HRESULT Client::MinimapRenderComponent::Initialize(void* arg)
{

	CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	if (arg)
	{
		MINIMAPDESC* pMinimapDesc = static_cast<MINIMAPDESC*>(arg);
		m_eIconType = pMinimapDesc->eType;
		m_bIsTrace = pMinimapDesc->m_bIsTrace;
		m_pOwner = pMinimapDesc->pOwner;
		m_bUseRotation = pMinimapDesc->m_bUseRotation;

		if (m_pOwner)
		{
			m_MinimapInfo.m_pTargetComponent = this;
			m_MinimapInfo.m_pTransform = m_pOwner->Get_Transform();
		}
		
		wstring BaseTexPath = L"../../Resources/UI/Textures/Common/Textures/";
		switch (m_eIconType)
		{
		case Client::MinimapRenderComponent::PLAYER:
		{
			m_pMinimapManager->Set_PlayerTransform(m_MinimapInfo.m_pTransform);
			m_pTraceTexSRV = m_pGameInstance->Load_Texture(BaseTexPath + L"DiamondIcon_FootStep.png");//발자국
			m_pIconTexSRV = m_pGameInstance->Load_Texture(BaseTexPath + L"T_RadarMapArrow_UI.png");
		
		
		}
			
			break;
		case Client::MinimapRenderComponent::MONSTER:
		{
			m_pIconTexSRV = m_pGameInstance->Load_Texture(BaseTexPath + L"T_RadarMapEnemy_UI.png");

		}
			break;

		case Client::MinimapRenderComponent::BOSS:
		{
			m_fIconScale = 0.3f;
			m_pIconTexSRV = m_pGameInstance->Load_Texture(BaseTexPath + L"T_RadarMapEnemyStrong_UI.png");

		}
		break;

		case Client::MinimapRenderComponent::LADDER:
			m_pIconTexSRV = m_pGameInstance->Load_Texture(BaseTexPath + L"T_RadarMapLadder_UI.png");
			break;
		case Client::MinimapRenderComponent::ITEM:
			m_pIconTexSRV = m_pGameInstance->Load_Texture(BaseTexPath + L"T_RadarMapItem_UI.png");
			break;

		case Client::MinimapRenderComponent::SAVEPOINT:
		{
			m_fIconScale = 0.2f;
			m_pIconTexSRV = m_pGameInstance->Load_Texture(BaseTexPath + L"T_RadarMapCheckPoint_Open_UI.png");

		}
		break;

		case Client::MinimapRenderComponent::END:
			break;
		default:
			break;
		}
	}

	

	return S_OK;
}

//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::MinimapRenderComponent::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

	//매니저 큐에 등록시키는작업
	if(m_bIsTrace)
		m_pMinimapManager->Add_Minimap_TraceQueue(m_MinimapInfo);
	
	if(m_eIconType!=ICON_TYPE::PLAYER)
		m_pMinimapManager->Add_Minimap_CommonQueue(m_MinimapInfo);

	else
		m_pMinimapManager->Add_Minimap_PlayerData(m_MinimapInfo);

    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/


HRESULT Client::MinimapRenderComponent::Render(const _float fTimeDelta)
{

    return S_OK;
}

void Client::MinimapRenderComponent::Render_Trace(_float2 UV, Shader* pShader, const _float fTimeDelta)
{
	//계쏙남는 잔상을그릴때
	CHECK_FALSE(m_bIsTrace);
	CHECK_JUST_NULL(pShader);
	CHECK_JUST_NULL(m_pTraceTexSRV);

	//TargetUV-Manager로부터 요청
	float  Rotation = 0.f;



	pShader->Bind_RawValue_ByHandle(g_vTargetUV, &UV, sizeof(_float2));
	pShader->Bind_RawValue_ByHandle(g_fRotation, &Rotation, sizeof(float));
	pShader->Bind_RawValue_ByHandle(g_fIconScale, &g_fTraceIconScale, sizeof(float));
	pShader->Bind_SRV_FullSlot(1, m_pTraceTexSRV);

	//0->STAMP PASS
	pShader->Begin(1);
	pShader->Bind_Resources(1);

	m_pMinimapManager->Bind_Resource_By_Buffer();
	m_pMinimapManager->Render_By_Buffer(fTimeDelta);
}

void Client::MinimapRenderComponent::Render_Icon(_float fCameraYaw,_float2 UV,_float fZoom, Shader* pShader, const _float fTimeDelta)
{
	CHECK_JUST_NULL(pShader);
	CHECK_JUST_NULL(m_pIconTexSRV);
	CHECK_JUST_NULL(m_MinimapInfo.m_pTransform);

	//m_pGameInstance->Set_DepthStencilState(DSSTATE::NODEPTH);
	//TargetUV-Manager로부터 요청
	float  Rotation = 0.f;
	
	if (m_bUseRotation)
	{
		Rotation = m_MinimapInfo.m_pTransform->Get_Rotation_Yaw()- fCameraYaw;
		
	}

	pShader->Bind_RawValue_ByHandle(g_fRotation, &Rotation, sizeof(float));
	_float fDynamicScale = m_fIconScale / fZoom;
	pShader->Bind_RawValue_ByHandle(g_fIconScale, &fDynamicScale, sizeof(float));
	pShader->Bind_SRV_FullSlot(1, m_pIconTexSRV);

	//0->STAMP PASS
	pShader->Begin(1);
	pShader->Bind_Resources(1);


	m_pMinimapManager->Bind_Resource_By_Buffer();
	m_pMinimapManager->Render_By_Buffer(fTimeDelta);

//	m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
}


MinimapRenderComponent* Client::MinimapRenderComponent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	MinimapRenderComponent* pInstance = new MinimapRenderComponent(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"UIMinimapRenderComponent_Image 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Client::MinimapRenderComponent::Clone(void* arg)
{
	MinimapRenderComponent* pInstance = new MinimapRenderComponent(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"MinimapRenderComponent 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}


void Client::MinimapRenderComponent::Free()
{
	Safe_Release(m_pTraceTexSRV);
	Safe_Release(m_pIconTexSRV);

    __super::Free();
}
