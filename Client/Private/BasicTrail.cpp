#include "Client_Define.h"
#include "BasicTrail.h"
#include "VIBuffer_Trail.h"

BasicTrail::BasicTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : TrailEffect(pDevice, pContext)
{
}

BasicTrail::BasicTrail(const BasicTrail& original)
    : TrailEffect(original)
{
}

HRESULT BasicTrail::Initialize_Prototype()
{
    return S_OK;
}

HRESULT BasicTrail::Initialize(void* pArg)
{
    m_wstrName = TEXT("Effect_BasicTrail");

    CHECK_FAILED(TrailEffect::Initialize(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(), E_FAIL);

    m_bIsActive = false;

    return S_OK;
}

_int BasicTrail::Update_Priority(const _float fTimeDelta)
{
    TrailEffect::Update_Priority(fTimeDelta);

    return 0;
}

_int BasicTrail::Update(const _float fTimeDelta)
{
    m_fAccumulatedTime += fTimeDelta;

    TrailEffect::Update(fTimeDelta);

    return 0;
}

_int BasicTrail::Update_Late(const _float fTimeDelta)
{
    TrailEffect::Update_Late(fTimeDelta);

    return 0;
}

HRESULT BasicTrail::Render(const _float fTimeDelta)
{
    CHECK_FAILED(Bind_ShaderResources(), E_FAIL);

    CHECK_FAILED(TrailEffect::Render(fTimeDelta), E_FAIL);

    return S_OK;
}

HRESULT BasicTrail::Ready_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Trail"),
        TEXT("Com_VIBuffer"), reinterpret_cast<Component**>(&m_pTrailBuffer))))
        return E_FAIL;

    /* For.Com_Shader */
    _wstring shaderTag = m_strShaderName.empty()
        ? L"Prototype_Component_Shader_Trail"
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

HRESULT BasicTrail::Bind_ShaderResources()
{
    CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 0, m_iTextureNum), E_FAIL);
    CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 1, m_iNoiseTextureNum), E_FAIL);

    // Mask Texture
    _uint bUseMask = m_strMaskTextureName.empty() ? 0 : 1;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseMask, &bUseMask, sizeof(_uint)), E_FAIL);
    if (bUseMask)
    {
        CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 3, m_iMaskTextureNum), E_FAIL);
    }

    CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fTransparency, &m_fAccumulatedTime, sizeof(_float)), E_FAIL);             // 누적 시간
    CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fDistortionStrength, &m_fDistortionStrength, sizeof(_float)), E_FAIL);    // 왜곡값

    CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_vColor, &m_vColor, sizeof(_float4)), E_FAIL);

    return S_OK;
}

HRESULT BasicTrail::Change_Texture(const _wstring strFilePath)
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

HRESULT BasicTrail::Change_NoiseTexture(const _wstring strFilePath)
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

HRESULT Client::BasicTrail::Change_MaskTexture(const _wstring strFilePath)
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

HRESULT BasicTrail::Change_Shader(const _wstring strShaderTag)
{
    // 기존의 쉐이더 해제
    Safe_Release(m_pShaderCom);

    // 새로운 쉐이더 복제
    m_pShaderCom = static_cast<Shader*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, _UINT(LEVEL::STATIC), strShaderTag));

    m_umapComponents[TEXT("Com_Shader")] = m_pShaderCom;

    m_strShaderName = wstringToString(strShaderTag);

    return S_OK;
}

BasicTrail* BasicTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    BasicTrail* pInstance = new BasicTrail(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : BasicTrail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

GameObject* BasicTrail::Clone(void* pArg)
{
    _bool bHasData = (m_pTrailBuffer != nullptr);
    VIBuffer_Trail::TRAIL_DESC savedTrailDesc{};
    if (bHasData)
        savedTrailDesc = Get_TrailDesc();

    BasicTrail* pInstance = new BasicTrail(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : BasicTrail");
        Safe_Release(pInstance);
        return nullptr;
    }

    if (bHasData)
    {
        pInstance->Set_TrailDesc(savedTrailDesc);
        pInstance->Set_PassIndex(m_iPassIndex);
    }

    return pInstance;
}

void BasicTrail::Free()
{
    __super::Free();
}
