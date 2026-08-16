#include "Client_Define.h"
#include "BasicParticle.h"
#include "VIBuffer_Particle_Rect.h"
#include "SubEmitter.h"

BasicParticle::BasicParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: ParticleEffect{ pDevice, pContext }
{
}

BasicParticle::BasicParticle(const BasicParticle& Prototype)
	: ParticleEffect{ Prototype }
{
}

HRESULT BasicParticle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT BasicParticle::Initialize(void* pArg)
{
	m_wstrName = TEXT("Effect_Basic");

	if (FAILED(ParticleEffect::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_bIsActive = false;

	return S_OK;
}

_int BasicParticle::Update_Priority(const _float fTimeDelta)
{
	ParticleEffect::Update_Priority(fTimeDelta);

	return 0;
}

_int BasicParticle::Update(const _float fTimeDelta)
{
	// Compute Shading 바인드 함수
	CHECK_FAILED(CAST(VIBuffer_Particle_Rect*)(m_pVIBufferCom)->Bind_ComputeShaderResources(m_pComputeShaderCom, fTimeDelta), E_FAIL);

	ParticleEffect::Update(fTimeDelta);

	return 0;
}

_int BasicParticle::Update_Late(const _float fTimeDelta)
{
	ParticleEffect::Update_Late(fTimeDelta);

	return 0;
}

HRESULT BasicParticle::Render(const _float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_vecObjPass[0])))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[0])))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render(0.f)))
		return E_FAIL;

	ParticleEffect::Render(fTimeDelta);

	//렌더 끝나고나서 이전 월드 넣어주기
	m_pTransformCom->Set_PrevMatrix_Float4x4(m_pTransformCom->Get_WorldFloat4x4());

	return S_OK;
}

HRESULT BasicParticle::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Particle_Basic"),
		TEXT("Com_VIBuffer"), reinterpret_cast<Component**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_ComputeShader */
	_wstring computeShaderTag = m_strComputeShaderName.empty()
		? L"Prototype_Component_Shader_Compute_Particle"
		: stringToWstring(m_strComputeShaderName);
	if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), computeShaderTag,
		TEXT("Com_ComputeShader"), reinterpret_cast<Component**>(&m_pComputeShaderCom))))
		return E_FAIL;
	m_strComputeShaderName = wstringToString(computeShaderTag);

	/* For.Com_Shader */
	_wstring shaderTag = m_strShaderName.empty()
		? L"Prototype_Component_Shader_Particle_Rect"
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

HRESULT BasicParticle::Bind_ShaderResources()
{
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fTimeElapsed, &m_fAccumulatedTime, sizeof(_float)), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vFadeInOut, &m_vFadeInOut, sizeof(_float2)), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bEmissive, &m_bEmissive, sizeof(_uint)), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vEffectOffset, &m_vEffectOffset, sizeof(_float2)), E_FAIL);

	// Texture
	_uint bUseTexture = m_strTextureName.empty() ? 0 : 1;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseTexture, &bUseTexture, sizeof(_uint)), E_FAIL);
	if (bUseTexture)
	{
		CHECK_FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vDiffuseUVScroll, &m_vDiffuseUVScroll, sizeof(_float2)), E_FAIL);
	}

	// Mask Texture
	_uint bUseMask = m_strMaskTextureName.empty() ? 0 : 1;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseMask, &bUseMask, sizeof(_uint)), E_FAIL);
	if (bUseMask)
	{
		CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 1, m_iMaskTextureNum), E_FAIL);
	}

	// Noise Texture
	_uint bUseNoise = m_strNoiseTextureName.empty() ? 0 : 1;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseNoise, &bUseNoise, sizeof(_uint)), E_FAIL);
	if (bUseNoise)
	{
		CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 3, m_iNoiseTextureNum), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fDistortionStrength, &m_fDistortionStrength, sizeof(_float)), E_FAIL);

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

	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_EffectWorldMatrix), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_PrevWorldMatrix, m_pTransformCom->Get_PrevWorldFloat4x4()), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vColor, &m_vColor, sizeof(_float4)), E_FAIL);

	CHECK_FAILED(m_pVIBufferCom->Bind_EffectResources(m_pShaderCom), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fMaskIntensity, &m_fMaskIntensity, sizeof(_float)), E_FAIL);
	return S_OK;
}

HRESULT BasicParticle::Change_Texture(const _wstring strFilePath)
{
	// 단어 시작 위치 찾기
	size_t posStart = strFilePath.find(L"Effects");

	// _wstring::npos : _wstring.find 에서 특정 문자열을 찾지 못했을 때 반환값
	if (posStart != _wstring::npos)
	{
		// Textures 부터 끝까지 추출
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

HRESULT BasicParticle::Change_MaskTexture(const _wstring strFilePath)
{
	// 단어 시작 위치 찾기
	size_t posStart = strFilePath.find(L"Effects");

	// _wstring::npos : _wstring.find 에서 특정 문자열을 찾지 못했을 때 반환값
	if (posStart != _wstring::npos)
	{
		// Textures 부터 끝까지 추출
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

HRESULT BasicParticle::Change_AlphaMaskTexture(const _wstring strFilePath)
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

HRESULT BasicParticle::Change_NoiseTexture(const _wstring strFilePath)
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

HRESULT BasicParticle::Change_Shader(const _wstring strShaderTag)
{
	// 기존의 쉐이더 해제
	Safe_Release(m_pShaderCom);

	// 새로운 쉐이더 복제
	m_pShaderCom = static_cast<Shader*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, _UINT(LEVEL::STATIC), strShaderTag));

	m_umapComponents[TEXT("Com_Shader")] = m_pShaderCom;

	m_strShaderName = wstringToString(strShaderTag);

	return S_OK;
}

HRESULT BasicParticle::Change_Compute_Shader(const _wstring strShaderTag)
{
	// 기존의 쉐이더 해제
	Safe_Release(m_pComputeShaderCom);

	// 새로운 쉐이더 복제
	m_pComputeShaderCom = static_cast<ComputeShader*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, _UINT(LEVEL::STATIC), strShaderTag));

	m_umapComponents[TEXT("Com_ComputeShader")] = m_pComputeShaderCom;

	m_strComputeShaderName = wstringToString(strShaderTag);

	return S_OK;
}

BasicParticle* BasicParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	BasicParticle* pInstance = new BasicParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : BasicParticle");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

GameObject* BasicParticle::Clone(void* pArg)
{
	VIBuffer_Particle::PARTICLE_DESC savedDesc{};
	_bool bHasData = (m_pVIBufferCom != nullptr);
	if (bHasData)
		savedDesc = m_pVIBufferCom->Get_ParticleDesc();

	BasicParticle* pInstance = new BasicParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BasicParticle");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (bHasData)
	{
		CAST(VIBuffer_Particle_Rect*)(pInstance->m_pVIBufferCom)->Update_Particles(savedDesc);
		pInstance->Set_PassIndex(m_iPassIndex);

		if (m_vRotation.x != 0.f || m_vRotation.y != 0.f || m_vRotation.z != 0.f)
			pInstance->Set_EffectRotation(m_vRotation);
	}

	return pInstance;
}

void BasicParticle::Free()
{
	__super::Free();
}
