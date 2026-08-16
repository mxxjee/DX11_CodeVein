#include "Engine_Define.h"
#include "LightComponent.h"

#include "GameInstance.h"
#include "LightManager.h"
#include "Light.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::LightComponent::LightComponent()
{
}

Engine::LightComponent::LightComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Component(pDevice, pContext)
{
}

Engine::LightComponent::LightComponent(const LightComponent& original)
    : Component(original), m_LightDesc(original.m_LightDesc)
{
}

Engine::LightComponent::~LightComponent()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::LightComponent::Initialize_Prototype(LIGHT_DESC& Desc)
{
    //m_LightDesc = new LIGHT_DESC;
    m_LightDesc = Desc;
    return S_OK;
}

HRESULT Engine::LightComponent::Initialize(void* _arg)
{
    if (_arg != nullptr)
    {
        m_LightDesc = *static_cast<LIGHT_DESC*>(_arg);
    }

    //LIGHT_DESC* desc = CAST(LIGHT_DESC*)(_arg);

  /*  switch (m_LightDesc.eType)
    {
    case LIGHT::DEBUG:
        m_pLight = Light::Create(m_pDevice, m_pContext, m_LightDesc);
        break;
    case LIGHT::DIRECTIONAL:
        m_pLight = Light::Create(m_pDevice, m_pContext, m_LightDesc);
        break;
    case LIGHT::POINT:
        m_pLight = Light::Create(m_pDevice, m_pContext, m_LightDesc);
        break;
    }*/ 
    m_iLightIndex = m_pGameInstance->Get_LightCnt();
    m_pGameInstance->Add_Light(m_iLightIndex, m_LightDesc);
    m_pLight = m_pGameInstance->Get_LightPtr(m_iLightIndex);
    m_pMatrix = XMMatrixIdentity();

    m_fBaseRange = m_LightDesc.fRange;
    m_vBaseDiffuse = m_LightDesc.vDiffuse;

    m_LightDesc.bIsVisible = m_bIsVisible;
    m_pLight->Set_LightDesc(m_LightDesc);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/

//////////////////////////////////////////////////////// 위치가져오기 함수 ////////////////////////////////////////////////////////
_matrix& Engine::LightComponent::Get_Light_Matrix()
{
    LIGHT_DESC* desc = m_pLight->Get_LightDesc();
    m_pMatrix.r[3] = XMLoadFloat4(&desc->vPosition);
    
    return m_pMatrix;
}
/******************************************************* 위치가져오기 함수 *******************************************************/

//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::LightComponent::Update_Priority(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::LightComponent::Update(const _float fTimeDelta, _vector fPosition)
{
    if (!m_bIsVisible)
        return 0;

    XMStoreFloat4(&m_LightDesc.vPosition, fPosition);

    if (m_bFlicker)
    {
        m_fFlickerTime += fTimeDelta * m_fFlickerSpeed;

        _float fNoise = (sinf(m_fFlickerTime) + cosf(m_fFlickerTime * 1.3f) + sinf(m_fFlickerTime * 2.7f)) / 3.0f;
        fNoise = (fNoise + 1.0f) * 0.5f;

        _float fScale = m_fFlickerMin + fNoise * (m_fFlickerMax - m_fFlickerMin);

        m_LightDesc.fRange = m_fBaseRange * fScale;
        m_LightDesc.vDiffuse.x = m_vBaseDiffuse.x * fScale;
        m_LightDesc.vDiffuse.y = m_vBaseDiffuse.y * fScale;
        m_LightDesc.vDiffuse.z = m_vBaseDiffuse.z * fScale;
        m_LightDesc.fPlayerDistance = m_pLight->Get_LightDesc()->fPlayerDistance;
    }

    m_pLight->Set_LightDesc(m_LightDesc);
    return 0;
}

/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::LightComponent::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::LightComponent::Render(const _float fTimeDelta)
{
    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/


void Engine::LightComponent::Set_Visible(_bool _bVisible)
{
    m_bIsVisible = _bVisible;
    m_LightDesc.bIsVisible = _bVisible;

    if (m_pLight != nullptr)
    {
        m_pLight->Set_LightDesc(m_LightDesc);
    }
}


//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
LightComponent* Engine::LightComponent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LIGHT_DESC& Desc)
{
    LightComponent* pInstance = new LightComponent(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(Desc), L"LightComponent 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

LightComponent* Engine::LightComponent::Clone(void* arg)
{
    LightComponent* pInstance = new LightComponent(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"LightComponent 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::LightComponent::Free()
{
    if (m_pGameInstance != nullptr && m_iLightIndex != -1)
    {
        m_pGameInstance->Delete_Light(m_iLightIndex);
        m_iLightIndex = -1;
    }

    m_pLight = nullptr;
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/

