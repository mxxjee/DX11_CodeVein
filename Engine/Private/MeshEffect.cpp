#include "Engine_Define.h" 
#include "MeshEffect.h"

Engine::MeshEffect::MeshEffect()
{
}

Engine::MeshEffect::MeshEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: ParticleEffect(pDevice, pContext)
{
}

Engine::MeshEffect::MeshEffect(const MeshEffect& original)
	: ParticleEffect(original)
	, m_vPosition{original.m_vPosition}
	, m_vScale{original.m_vScale}
	, m_fMeshScale{original.m_fMeshScale}
	, m_fScaleDelay{original.m_fScaleDelay}
	, m_iScaleAxis{original.m_iScaleAxis}
	, m_iRotateRight{original.m_iRotateRight}
	, m_iRotateUp{original.m_iRotateUp}
	, m_iRotateLook{original.m_iRotateLook}
	, m_iFrameCount{original.m_iFrameCount}
	, m_iCountX{original.m_iCountX}
	, m_iCountY{original.m_iCountY}
	, m_fDisplaceStrength{original.m_fDisplaceStrength}
	, m_iMaskStartFrame{original.m_iMaskStartFrame}
	, m_iMaskFrameCount{original.m_iMaskFrameCount}
	, m_iMaskCountX{original.m_iMaskCountX}
	, m_iMaskCountY{original.m_iMaskCountY}
	, m_vMaskUVScroll{original.m_vMaskUVScroll}
	, m_vMaskUVScale{original.m_vMaskUVScale}
	, m_vMaskUVStartOffset{original.m_vMaskUVStartOffset}
	, m_bDirectionalDissolve{original.m_bDirectionalDissolve}
	, m_bDissolveFlipY{original.m_bDissolveFlipY}
	, m_fNoiseWeight{original.m_fNoiseWeight}
	, m_fDissolveEdgeWidth{original.m_fDissolveEdgeWidth}
	, m_vDissolveEdgeColor{original.m_vDissolveEdgeColor}
	, m_bAlignToPlayerLook{original.m_bAlignToPlayerLook}
{
}

MeshEffect::MESHEFFECT_DESC Engine::MeshEffect::Get_MeshDesc()
{
	MESHEFFECT_DESC meshDesc{};

	meshDesc.vPosition = m_vPosition;
	meshDesc.vScale = m_vScale;
	meshDesc.fMeshScale = m_fMeshScale;
	meshDesc.fScaleDelay = m_fScaleDelay;
	meshDesc.iScaleAxis = m_iScaleAxis;
	meshDesc.fLifeTime = m_fLifeTime;
	meshDesc.iRotateRight = m_iRotateRight;
	meshDesc.iRotateUp = m_iRotateUp;
	meshDesc.iRotateLook = m_iRotateLook;

	meshDesc.iFrameCount = m_iFrameCount;
	meshDesc.iCountX = m_iCountX;
	meshDesc.iCountY = m_iCountY;
	meshDesc.fDisplaceStrength = m_fDisplaceStrength;

	meshDesc.iMaskStartFrame = m_iMaskStartFrame;
	meshDesc.iMaskFrameCount = m_iMaskFrameCount;
	meshDesc.iMaskCountX = m_iMaskCountX;
	meshDesc.iMaskCountY = m_iMaskCountY;
	meshDesc.vMaskUVScroll = m_vMaskUVScroll;
	meshDesc.vMaskUVScale = m_vMaskUVScale;
	meshDesc.vMaskUVStartOffset = m_vMaskUVStartOffset;

	meshDesc.bDirectionalDissolve = m_bDirectionalDissolve;
	meshDesc.bDissolveFlipY = m_bDissolveFlipY;
	meshDesc.fNoiseWeight = m_fNoiseWeight;
	meshDesc.fDissolveEdgeWidth = m_fDissolveEdgeWidth;
	meshDesc.vDissolveEdgeColor = m_vDissolveEdgeColor;

	meshDesc.bAlignToPlayerLook = m_bAlignToPlayerLook;
	return meshDesc;
}

void Engine::MeshEffect::Set_MeshDesc(const MESHEFFECT_DESC& desc)
{
	m_vPosition = desc.vPosition;
	m_vScale = desc.vScale;
	m_fMeshScale = desc.fMeshScale;
	m_fScaleDelay = desc.fScaleDelay;
	m_iScaleAxis = desc.iScaleAxis;

	m_fLifeTime = desc.fLifeTime;
	m_iRotateRight = desc.iRotateRight;
	m_iRotateUp = desc.iRotateUp;
	m_iRotateLook = desc.iRotateLook;
	m_fDisplaceStrength = desc.fDisplaceStrength;

    // 스프라이트 이미지 변수 설정
    m_iFrameCount = desc.iFrameCount;
    m_iCountX = desc.iCountX;
    m_iCountY = desc.iCountY;

    // 마스크 스프라이트 변수 설정
    m_iMaskStartFrame = desc.iMaskStartFrame;
    m_iMaskFrameCount = desc.iMaskFrameCount;
    m_iMaskCountX = desc.iMaskCountX;
    m_iMaskCountY = desc.iMaskCountY;
    m_vMaskUVScroll = desc.vMaskUVScroll;
    m_vMaskUVScale = desc.vMaskUVScale;
	m_vMaskUVStartOffset = desc.vMaskUVStartOffset;

	// Dissolve
	m_bDirectionalDissolve = desc.bDirectionalDissolve;
	m_bDissolveFlipY = desc.bDissolveFlipY;
	m_fNoiseWeight = desc.fNoiseWeight;
	m_fDissolveEdgeWidth = desc.fDissolveEdgeWidth;
	m_vDissolveEdgeColor = desc.vDissolveEdgeColor;
	m_bAlignToPlayerLook = desc.bAlignToPlayerLook;
	Apply_WorldOffset();
}

HRESULT Engine::MeshEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::MeshEffect::Initialize(void* pArg)
{
	CHECK_FAILED(ParticleEffect::Initialize(pArg), E_FAIL);

	return S_OK;
}

_int Engine::MeshEffect::Update_Priority(const _float fTimeDelta)
{
	ParticleEffect::Update_Priority(fTimeDelta);

	return 0;
}

_int Engine::MeshEffect::Update(const _float fTimeDelta)
{
	ParticleEffect::Update(fTimeDelta);

	if (m_bAlignToPlayerLook)
	{
		GameObject* pPlayer = m_pGameInstance->Get_Player();
		if (pPlayer != nullptr)
		{
			Transform* pPlayerTransform = pPlayer->Get_Transform();

			_float3 vRot = Get_EffectRotation();
			_matrix matLocalRot = XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(vRot.x),
				XMConvertToRadians(vRot.y),
				XMConvertToRadians(vRot.z)
			);

			_vector vPlayerRight = XMVector3Normalize(pPlayerTransform->Get_State(DIRECTION::RIGHT));
			_vector vPlayerUp = XMVector3Normalize(pPlayerTransform->Get_State(DIRECTION::UP));
			_vector vPlayerLook = XMVector3Normalize(pPlayerTransform->Get_State(DIRECTION::LOOK));

			_matrix matPlayerRot = XMMatrixIdentity();
			matPlayerRot.r[0] = vPlayerRight;
			matPlayerRot.r[1] = vPlayerUp;
			matPlayerRot.r[2] = vPlayerLook;

			_matrix matFinalRot = matLocalRot * matPlayerRot;

			m_pTransformCom->Set_State(DIRECTION::RIGHT, matFinalRot.r[0]);
			m_pTransformCom->Set_State(DIRECTION::UP, matFinalRot.r[1]);
			m_pTransformCom->Set_State(DIRECTION::LOOK, matFinalRot.r[2]);
		}
	}

	// 수명 비율에 따라서 크기를 보간
	//_float fLifeRatio = saturate(m_fAccumulatedTime / m_fLifeTime);

	_float fScaleRatio = 0.f;
	if (m_fAccumulatedTime > m_fScaleDelay)
		fScaleRatio = saturate((m_fAccumulatedTime - m_fScaleDelay) / (m_fLifeTime - m_fScaleDelay));
	_float fCurrentScale = fLerp(1.f, m_fMeshScale, fScaleRatio);

	_float fScaleX = m_vScale.x * ((m_iScaleAxis & 1) ? fCurrentScale : 1.f);
	_float fScaleY = m_vScale.y * ((m_iScaleAxis & 2) ? fCurrentScale : 1.f);
	_float fScaleZ = m_vScale.z * ((m_iScaleAxis & 4) ? fCurrentScale : 1.f);
	m_pTransformCom->Set_Scale(fScaleX, fScaleY, fScaleZ);

	if (m_iRotateRight != 0)
		m_vAccumulatedRotation.x += m_iRotateRight * fTimeDelta;
	if (m_iRotateUp != 0)
		m_vAccumulatedRotation.y += m_iRotateUp * fTimeDelta;
	if (m_iRotateLook != 0)
		m_vAccumulatedRotation.z += m_iRotateLook * fTimeDelta;

	return 0;
}

_int Engine::MeshEffect::Update_Late(const _float fTimeDelta)
{
	ParticleEffect::Update_Late(fTimeDelta);

	return 0;
}

HRESULT Engine::MeshEffect::Render(const _float fTimeDelta)
{
	ParticleEffect::Render(fTimeDelta);

	return S_OK;
}

void Engine::MeshEffect::Apply_WorldOffset()
{
	// 수명 비율에 따라서 크기를 보간
	//_float fLifeRatio = (m_fLifeTime > 0.f) ? saturate(m_fAccumulatedTime / m_fLifeTime) : 0.f;
	_float fScaleRatio = 0.f;
	if (m_fAccumulatedTime > m_fScaleDelay)
		fScaleRatio = saturate((m_fAccumulatedTime - m_fScaleDelay) / (m_fLifeTime - m_fScaleDelay));
	_float fCurrentScale = fLerp(1.f, m_fMeshScale, fScaleRatio);

	// 총 회전 = 초기 회전(m_vRotation) + 누적 회전
	_float fRotX = XMConvertToRadians(m_vRotation.x) + m_vAccumulatedRotation.x;
	_float fRotY = XMConvertToRadians(m_vRotation.y) + m_vAccumulatedRotation.y;
	_float fRotZ = XMConvertToRadians(m_vRotation.z) + m_vAccumulatedRotation.z;

	_float fScaleX = m_vScale.x * ((m_iScaleAxis & 1) ? fCurrentScale : 1.f);
	_float fScaleY = m_vScale.y * ((m_iScaleAxis & 2) ? fCurrentScale : 1.f);
	_float fScaleZ = m_vScale.z * ((m_iScaleAxis & 4) ? fCurrentScale : 1.f);

	_matrix matLocal = XMMatrixScaling(fScaleX, fScaleY, fScaleZ)
		* XMMatrixRotationRollPitchYaw(fRotX, fRotY, fRotZ)
		* XMMatrixTranslation(m_vPosition.x, m_vPosition.y, m_vPosition.z);

	_matrix matParent = XMLoadFloat4x4(&m_matWorldOffset);
	m_pTransformCom->Set_Matrix(matLocal * matParent);
}

void Engine::MeshEffect::Reset_MeshSetting()
{
	m_vAccumulatedRotation = { 0.f, 0.f, 0.f };
}

void Engine::MeshEffect::Free()
{
	__super::Free();
}
