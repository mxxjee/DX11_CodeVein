#include "Client_Define.h"
#include "Decal_Blood.h"

Decal_Blood::Decal_Blood()
{
}

Decal_Blood::Decal_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Decal(pDevice, pContext)
{
}

Decal_Blood::Decal_Blood(const Decal_Blood& original)
	: Decal(original)
{
}

HRESULT Decal_Blood::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Decal_Blood::Initialize(void* pArg)
{
	CHECK_FAILED(Decal::Initialize(pArg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_bIsActive = false;

	m_vColor = _float4(0.157f, 0.f, 0.f, 1.f);

	return S_OK;
}

_int Decal_Blood::Update(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return 0;

	Decal::Update(fTimeDelta);

	return 0;
}

_int Decal_Blood::Update_Late(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return 0;

	Decal::Update_Late(fTimeDelta);

	return 0;
}

HRESULT Decal_Blood::Render(const _float fTimeDelta)
{
	CHECK_FAILED(Bind_ShaderResources(), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Begin(0), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_Resources(0), E_FAIL);
	CHECK_FAILED(m_pVIBufferCom->Bind_Resource(), E_FAIL);
	CHECK_FAILED(m_pVIBufferCom->Render(fTimeDelta), E_FAIL);

	return S_OK;
}

HRESULT Decal_Blood::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<Component**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Shader_Decal"),
		TEXT("Com_Shader"), reinterpret_cast<Component**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Decal"),
		TEXT("Com_Texture"), reinterpret_cast<Component**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Decal_Mask"),
		TEXT("Com_MaskTexture"), reinterpret_cast<Component**>(&m_pMaskTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT Decal_Blood::Bind_ShaderResources()
{
	// b1: WorldMatrix
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);

	// b8: 데칼 역행렬
	_float4x4 matInvWorld{};
	XMStoreFloat4x4(&matInvWorld, m_pTransformCom->Get_InverseWorldMatrix());
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_InvDecalWorldMatrix, matInvWorld), E_FAIL);

	// b8: 알파
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fDecalAlpha, &m_fAlpha, sizeof(_float)), E_FAIL);

	// b8: 마스크 인덱스
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_iMaskIndex, &m_iMaskIndex, sizeof(_uint)), E_FAIL);

	// t0: 데칼 텍스처
	CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 0, 0), E_FAIL);

	// t1: 마스크 텍스처
	CHECK_FAILED(m_pMaskTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 1, 0), E_FAIL);

	// 색상 변경
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vColor, &m_vColor, sizeof(_float4)), E_FAIL);

	return S_OK;
}

Decal_Blood* Decal_Blood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Decal_Blood* pInstance = new Decal_Blood(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Decal_Blood");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject* Decal_Blood::Clone(void* pArg)
{
	Decal_Blood* pInstance = new Decal_Blood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Decal_Blood");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Decal_Blood::Free()
{
	__super::Free();
}
