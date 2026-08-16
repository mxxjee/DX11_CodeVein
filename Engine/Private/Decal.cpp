#include "Engine_Define.h"
#include "Decal.h"

Decal::Decal()
{
}

Decal::Decal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{
}

Decal::Decal(const Decal& original)
	: GameObject(original)
{
}

HRESULT Decal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Decal::Initialize(void* pArg)
{
	CHECK_FAILED(GameObject::Initialize(pArg), E_FAIL);

	auto* pDesc = CAST(DECAL_DESC*)(pArg);
	
	m_fMaxLifeTime = pDesc->fMaxLifeTime;
	m_iTexIndex = pDesc->iTexIndex; 
	m_iMaskIndex = (_uint)m_pGameInstance->RandomValue(0.f, 3.99f);

	m_pTransformCom->Set_State(DIRECTION::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);

	m_bIsActive = false;
	
	return S_OK;
}

_int Decal::Update(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return 0;

	m_fLifeTime += fTimeDelta;
	if (m_fLifeTime > m_fMaxLifeTime)
		return -1;

	_float fFadeStart = m_fMaxLifeTime * 0.7f;
	if (m_fLifeTime > fFadeStart)
		m_fAlpha = 1.f - (m_fLifeTime - fFadeStart) / (m_fMaxLifeTime - fFadeStart);
	else
		m_fAlpha = 1.f;

	return 0;
}

_int Decal::Update_Late(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return 0;

	CHECK_FAILED(m_pGameInstance->Add_RenderObject(RENDER_GROUP::DECAL, this), E_FAIL);

	return 0;
}

HRESULT Decal::Render(const _float fTimeDelta)
{
	return S_OK;
}

void Decal::Reset(const DECAL_DESC& desc)
{
	m_fLifeTime = 0.f;
	m_fMaxLifeTime = desc.fMaxLifeTime;
	m_fAlpha = 1.f;
	m_iTexIndex = desc.iTexIndex;
	m_iMaskIndex = (_uint)m_pGameInstance->RandomValue(0.f, 3.99f);
	m_bIsActive = true;

	// 위치 설정
	m_pTransformCom->Set_State(DIRECTION::POSITION,
		XMVectorSetW(XMLoadFloat3(&desc.vPosition), 1.f));

	// 노멀 기반 회전 (데칼이 표면에 붙도록)
	_vector vUp = XMVector3Normalize(XMLoadFloat3(&desc.vNormal));
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vUp);
	if (XMVector3NearEqual(XMVector3Length(vRight), XMVectorZero(), XMVectorReplicate(0.001f)))
		vRight = XMVector3Cross(XMVectorSet(1.f, 0.f, 0.f, 0.f), vUp);
	vRight = XMVector3Normalize(vRight);
	_vector vLook = XMVector3Normalize(XMVector3Cross(vUp, vRight));

	// Rotation은 항등 행렬부터 시작해서 벽이나 경사면에서 정렬이 무시됨
	_matrix matRotation = XMMatrixRotationAxis(vUp, XMConvertToRadians(m_pGameInstance->RandomValue(-180.f, 180.f)));
	vRight = XMVector3TransformNormal(vRight, matRotation);
	vLook = XMVector3TransformNormal(vLook, matRotation);

	m_pTransformCom->Set_State(DIRECTION::RIGHT, vRight);
	m_pTransformCom->Set_State(DIRECTION::UP, vUp);
	m_pTransformCom->Set_State(DIRECTION::LOOK, vLook);

	m_pTransformCom->Set_Scale(desc.vScale.x, desc.vScale.y, desc.vScale.z);
}

Decal* Decal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

GameObject* Decal::Clone(void* pArg)
{
	return nullptr;
}

void Decal::Free()
{
	__super::Free();
}