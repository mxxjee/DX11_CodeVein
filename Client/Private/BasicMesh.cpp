#include "Client_Define.h"
#include "BasicMesh.h"

BasicMesh::BasicMesh()
{
}

BasicMesh::BasicMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: MeshEffect(pDevice, pContext)
{
}

BasicMesh::BasicMesh(const BasicMesh& original)
	: MeshEffect(original)
{
}

HRESULT BasicMesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT BasicMesh::Initialize(void* pArg)
{
	m_wstrName = TEXT("MeshEffect_Basic");

	MeshEffect::MESHEFFECT_DESC desc{};
	desc.fRotationSpeed = 1.f;
	desc.wstrModelName = m_wstrModelName;

	CHECK_FAILED(MeshEffect::Initialize(&desc), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_bIsActive = false;

	return S_OK;
}

_int BasicMesh::Update_Priority(const _float fTimeDelta)
{
	MeshEffect::Update_Priority(fTimeDelta);

	return 0;
}

_int BasicMesh::Update(const _float fTimeDelta)
{
	MeshEffect::Update(fTimeDelta);

	return 0;
}

_int BasicMesh::Update_Late(const _float fTimeDelta)
{
	MeshEffect::Update_Late(fTimeDelta);

	return 0;
}

HRESULT BasicMesh::Render(const _float fTimeDelta)
{
	MeshEffect::Render(fTimeDelta);

	CHECK_FAILED(Bind_ShaderResources(), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[0]), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[0]), E_FAIL);

	CHECK_FAILED(m_pModelCom->Render(0.f, 0), E_FAIL);

	//렌더 끝나고나서 이전 월드 넣어주기
	m_pTransformCom->Set_PrevMatrix_Float4x4(m_pTransformCom->Get_WorldFloat4x4());
	return S_OK;
}

HRESULT BasicMesh::Ready_Components()
{
	/* For.Com_Model */
	_wstring modelTag = m_wstrModelName.empty()
		? L"Prototype_Component_Model_SM_FX_GEZPlate04"
		: m_wstrModelName;
	CHECK_FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), modelTag,
		L"Com_Model", reinterpret_cast<Component**>(&m_pModelCom)), E_FAIL);
	m_wstrModelName = modelTag;

	/* For.Com_Shader */
	_wstring shaderTag = m_strShaderName.empty()
		? L"Prototype_Component_Shader_MeshEffect"
		: stringToWstring(m_strShaderName);
	if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), shaderTag,
		TEXT("Com_Shader"), reinterpret_cast<Component**>(&m_pShaderCom))))
		return E_FAIL;
	m_strShaderName = wstringToString(shaderTag);

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effects"),
		TEXT("Com_Texture"), reinterpret_cast<Component**>(&m_pTextureCom))))
		return E_FAIL;

	__super::Set_VecObjPassSize();
	__super::Set_AllPass_VecObjPass();

	return S_OK;
}

HRESULT BasicMesh::Bind_ShaderResources()
{
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fLifeTime, &m_fLifeTime, sizeof(_float)), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fTimeElapsed, &m_fAccumulatedTime, sizeof(_float)), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vFadeInOut, &m_vFadeInOut, sizeof(_float2)), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fDisplaceStrength, &m_fDisplaceStrength, sizeof(_float)), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fHotColor, &m_fHotColor, sizeof(_float)), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vHotColor, &m_vHotColor, sizeof(_float3)), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bEmissive, &m_bEmissive, sizeof(_uint)), E_FAIL);

	// Texture
	_uint bUseTexture = m_strTextureName.empty() ? 0 : 1;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseTexture, &bUseTexture, sizeof(_uint)), E_FAIL);
	if (bUseTexture)
	{
		CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 0, m_iTextureNum), E_FAIL);

		// 스프라이트 바인딩
		_float2 vScaleUV = _float2(1.f / (_float)m_iCountX, 1.f / (_float)m_iCountY);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vScaleUV, &vScaleUV, sizeof(_float2)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_iFrameCount, &m_iFrameCount, sizeof(_uint)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_iCountX, &m_iCountX, sizeof(_uint)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_iCountY, &m_iCountY, sizeof(_uint)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vDiffuseUVScroll, &m_vDiffuseUVScroll, sizeof(_float2)), E_FAIL);
	}

	// Mask Texture
	_uint bUseMask = m_strMaskTextureName.empty() ? 0 : 1;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseMask, &bUseMask, sizeof(_uint)), E_FAIL);
	if (bUseMask)
	{
		CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 1, m_iMaskTextureNum), E_FAIL);

		// 마스크 스프라이트 바인딩
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_iMaskStartFrame, &m_iMaskStartFrame, sizeof(_uint)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_iMaskFrameCount, &m_iMaskFrameCount, sizeof(_uint)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_iMaskCountX, &m_iMaskCountX, sizeof(_uint)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_iMaskCountY, &m_iMaskCountY, sizeof(_uint)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vMaskUVScroll, &m_vMaskUVScroll, sizeof(_float2)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vMaskUVScale, &m_vMaskUVScale, sizeof(_float2)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vMaskUVStartOffset, &m_vMaskUVStartOffset, sizeof(_float2)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_iMaskSampler, &m_iMaskSampler, sizeof(_uint)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fMaskIntensity, &m_fMaskIntensity, sizeof(_float)), E_FAIL);
	}

	// Noise Texture
	_uint bUseNoise = m_strNoiseTextureName.empty() ? 0 : 1;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseNoise, &bUseNoise, sizeof(_uint)), E_FAIL);
	if (bUseNoise)
	{
		CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 3, m_iNoiseTextureNum, stage_VS | stage_PS), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fDistortionStrength, &m_fDistortionStrength, sizeof(_float)), E_FAIL);

		// Directional Dissolve Option
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bDirectionalDissolve, &m_bDirectionalDissolve, sizeof(_uint)), E_FAIL);
		if (m_bDirectionalDissolve)
		{
			CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bDissolveFlipY, &m_bDissolveFlipY, sizeof(_uint)), E_FAIL);
			CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fNoiseWeight, &m_fNoiseWeight, sizeof(_float)), E_FAIL);
			CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fDissolveEdgeWidth, &m_fDissolveEdgeWidth, sizeof(_float)), E_FAIL);
			CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vDissolveEdgeColor, &m_vDissolveEdgeColor, sizeof(_float3)), E_FAIL);
		}

		// EgdeGlow 옵션
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseEdgeGlow, &m_bUseEdgeGlow, sizeof(_uint)), E_FAIL);
		if (m_bUseEdgeGlow)
		{
			CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fGlowStrength, &m_fGlowStrength, sizeof(_float)), E_FAIL);
		}
	}

	// Alpha Mask Texture
	_uint bUseAlphaMask = m_strAlphaMaskTextureName.empty() ? 0 : 1;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseAlphaMask, &bUseAlphaMask, sizeof(_uint)), E_FAIL);
	if (bUseAlphaMask)
	{
		CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 4, m_iAlphaMaskTextureNum), E_FAIL);
	}

	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_pTransformCom->Get_WorldFloat4x4()), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_PrevWorldMatrix, m_pTransformCom->Get_PrevWorldFloat4x4()), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vColor, &m_vColor, sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT BasicMesh::Change_Model(const _wstring strModelTag)
{
	Safe_Release(m_pModelCom);

	m_pModelCom = static_cast<Model*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, _UINT(LEVEL::STATIC), strModelTag));

	m_umapComponents[TEXT("Com_Model")] = m_pModelCom;

	m_wstrModelName = strModelTag;

	return S_OK;
}

HRESULT BasicMesh::Change_Texture(const _wstring strFilePath)
{
	// 단어 시작 위치 찾기
	size_t posStart = strFilePath.find(L"Effects");

	// _wstring::npos : _wstring.find 에서 특정 문자열을 찾지 못했을 때 반환값
	if (posStart != _wstring::npos)
	{
		// Effects 부터 끝까지 추출
		_wstring strCut = strFilePath.substr(posStart);

		// 문자열의 뒤에서 부터 검색해서 확장자의 . 제거
		size_t posExt = strCut.rfind(L".");
		if (posExt != _wstring::npos)
			strCut = strCut.substr(0, posExt);

		// 문자열에 \\ 이런식으로 되어있어서 교체함
		for (auto& wchar : strCut)
		{
			if (wchar == L'\\')
				wchar = L'/';
		}

		// string 으로 변환
		m_strTextureName = wstringToString(strCut);

		m_iTextureNum = m_pTextureCom->Get_TextureIdx(m_strTextureName);
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT BasicMesh::Change_MaskTexture(const _wstring strFilePath)
{
	// 단어 시작 위치 찾기
	size_t posStart = strFilePath.find(L"Effects");

	// _wstring::npos : _wstring.find 에서 특정 문자열을 찾지 못했을 때 반환값
	if (posStart != _wstring::npos)
	{
		// Effects 부터 끝까지 추출
		_wstring strCut = strFilePath.substr(posStart);

		// 문자열의 뒤에서 부터 검색해서 확장자의 . 제거
		size_t posExt = strCut.rfind(L".");
		if (posExt != _wstring::npos)
			strCut = strCut.substr(0, posExt);

		// 문자열에 \\ 이런식으로 되어있어서 교체함
		for (auto& wchar : strCut)
		{
			if (wchar == L'\\')
				wchar = L'/';
		}

		// string 으로 변환
		m_strMaskTextureName = wstringToString(strCut);

		m_iMaskTextureNum = m_pTextureCom->Get_TextureIdx(m_strMaskTextureName);
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT BasicMesh::Change_AlphaMaskTexture(const _wstring strFilePath)
{
	size_t posStart = strFilePath.find(L"Effects");

	if (posStart != _wstring::npos)
	{
		_wstring strCut = strFilePath.substr(posStart);

		size_t posExt = strCut.rfind(L".");
		if (posExt != _wstring::npos)
			strCut = strCut.substr(0, posExt);

		for (auto& wchar : strCut)
		{
			if (wchar == L'\\')
				wchar = L'/';
		}

		m_strAlphaMaskTextureName = wstringToString(strCut);

		m_iAlphaMaskTextureNum = m_pTextureCom->Get_TextureIdx(m_strAlphaMaskTextureName);
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT BasicMesh::Change_NoiseTexture(const _wstring strFilePath)
{
	size_t posStart = strFilePath.find(L"Effects");

	if (posStart != _wstring::npos)
	{
		_wstring strCut = strFilePath.substr(posStart);

		size_t posExt = strCut.rfind(L".");
		if (posExt != _wstring::npos)
			strCut = strCut.substr(0, posExt);

		for (auto& wchar : strCut)
		{
			if (wchar == L'\\')
				wchar = L'/';
		}

		m_strNoiseTextureName = wstringToString(strCut);

		m_iNoiseTextureNum = m_pTextureCom->Get_TextureIdx(m_strNoiseTextureName);
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT BasicMesh::Change_Shader(const _wstring strShaderTag)
{
	// 기존의 쉐이더 해제
	Safe_Release(m_pShaderCom);

	// 새로운 쉐이더 복제
	m_pShaderCom = static_cast<Shader*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, _UINT(LEVEL::STATIC), strShaderTag));

	m_umapComponents[TEXT("Com_Shader")] = m_pShaderCom;

	m_strShaderName = wstringToString(strShaderTag);

	return S_OK;
}

BasicMesh* BasicMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	BasicMesh* pInstance = new BasicMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : BasicMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject* BasicMesh::Clone(void* pArg)
{
	_bool bHasData = (m_pModelCom != nullptr);
	MESHEFFECT_DESC savedMeshDesc{};
	if (bHasData)
		savedMeshDesc = Get_MeshDesc();

	BasicMesh* pInstance = new BasicMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BasicMesh");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (bHasData)
	{
		if (m_vRotation.x != 0.f || m_vRotation.y != 0.f || m_vRotation.z != 0.f)
			pInstance->Set_EffectRotation(m_vRotation);

		pInstance->Set_MeshDesc(savedMeshDesc);
		pInstance->Set_PassIndex(m_iPassIndex);
	}

	return pInstance;
}

void BasicMesh::Free()
{
	__super::Free();
}
