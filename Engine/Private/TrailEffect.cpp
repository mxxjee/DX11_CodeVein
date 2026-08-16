#include "Engine_Define.h"
#include "TrailEffect.h"
#include "SubEmitter.h"

Engine::TrailEffect::TrailEffect()
{
}

Engine::TrailEffect::TrailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: MeshEffect(pDevice, pContext)
{
}

Engine::TrailEffect::TrailEffect(const TrailEffect& original)
	: MeshEffect(original)
	, m_iTrailPassIndex{original.m_iTrailPassIndex}
{
}

VIBuffer_Trail::TRAIL_DESC Engine::TrailEffect::Get_TrailDesc()
{
	if (m_pTrailBuffer != nullptr)
		return m_pTrailBuffer->Get_TrailDesc();

	return VIBuffer_Trail::TRAIL_DESC();
}

void Engine::TrailEffect::Set_TrailDesc(const VIBuffer_Trail::TRAIL_DESC& trailDesc)
{
	if (m_pTrailBuffer != nullptr)
		m_pTrailBuffer->Set_TrailDesc(trailDesc);
}

void Engine::TrailEffect::Set_Active(_bool _isActive)
{
	// 현재 활성화 중인데 비활성화 요청이 들어왔을 때
	if (_isActive == false && m_bIsActive)
	{
		m_bIsFading = true;
		return;
	}

	// 활성화 되는 경우
	__super::Set_Active(_isActive);
	if (_isActive)
		m_bIsFading = false;
}

HRESULT Engine::TrailEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::TrailEffect::Initialize(void* pArg)
{
	CHECK_FAILED(MeshEffect::Initialize(pArg), E_FAIL);

	return S_OK;
}

_int Engine::TrailEffect::Update_Priority(const _float fTimeDelta)
{
	return 0;
}

_int Engine::TrailEffect::Update(const _float fTimeDelta)
{
	if (m_pTrailBuffer)
	{
		m_pTrailBuffer->Update_Trail(fTimeDelta);

		// 페이딩 중이고 모든 점들이 사라져서 비워지면 비활성화
		if (m_bIsFading && m_pTrailBuffer->Get_IsDeqEmpty())
		{
			m_bIsFading = false;
			__super::Set_Active(false);
		}
	}

	return 0;
}

_int Engine::TrailEffect::Update_Late(const _float fTimeDelta)
{
	return 0;
}

HRESULT Engine::TrailEffect::Render(const _float fTimeDelta)
{
	m_pGameInstance->Set_RasterizerState(RSTATE::SOLID_NONE);

	CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[0]), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[0]), E_FAIL);

	CHECK_FAILED(m_pTrailBuffer->Bind_Resource(), E_FAIL);

	CHECK_FAILED(m_pTrailBuffer->Render(0.f), E_FAIL);

	MeshEffect::Render(0.f);

	return S_OK;
}

void Engine::TrailEffect::Add_TrailPoint(const _float3& vRoot, const _float3& vTip)
{
	if (m_bIsFading)
		return;

	if (m_pTrailBuffer != nullptr)
		m_pTrailBuffer->Add_Point(vRoot, vTip);

	// SubEmitter가 있으면 Trail의 가운데 위치에 Trigger
	if (m_pSubEmitter != nullptr)
	{
		_float3 vMiddlePoint = {};
		XMStoreFloat3(&vMiddlePoint, (XMLoadFloat3(&vRoot) + XMLoadFloat3(&vTip)) * 0.5f);

		m_pSubEmitter->Trigger_AtPosition(SubEmitter::SUBEMMITER_TYPE::BIRTH, vMiddlePoint);
	}
}

void Engine::TrailEffect::Reset_Trail()
{
	if (m_pTrailBuffer != nullptr)
		m_pTrailBuffer->Reset();
}

void Engine::TrailEffect::Free()
{
	__super::Free();
}
