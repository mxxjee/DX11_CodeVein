#include "Engine_Define.h"
#include "Light.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Light::Light()
{
}

Engine::Light::Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

Engine::Light::~Light()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Light::Initialize(const LIGHT_DESC& _lightdesc)
{
	m_LightDesc = _lightdesc;

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Light::Render(Shader* _shader, VIBuffer_Rect* _buffer, const _float fTimeDelta)
{
    _uint passIndex = 0;

    switch (m_LightDesc.eType)
    {
        case LIGHT::DIRECTIONAL:
            _shader->Bind_RawValue_ByHandle(g_vLightDirection, &m_LightDesc.vDirection, sizeof(_float4));
            passIndex = _UINT(LIGHT::DIRECTIONAL_PBR);
            break;
        case LIGHT::POINT:
            _shader->Bind_RawValue_ByHandle(g_vLightPosition, &m_LightDesc.vPosition, sizeof(_float4));
            _shader->Bind_RawValue_ByHandle(g_fLightRange, &m_LightDesc.fRange, sizeof(_float));
            passIndex = _UINT(LIGHT::POINT);
            break;
        case LIGHT::SPOTLIGHT:
            _shader->Bind_RawValue_ByHandle(g_vLightDirection, &m_LightDesc.vDirection, sizeof(_float4));
            _shader->Bind_RawValue_ByHandle(g_vLightPosition, &m_LightDesc.vPosition, sizeof(_float4));
            _shader->Bind_RawValue_ByHandle(g_fLightRange, &m_LightDesc.fRange, sizeof(_float));
            _shader->Bind_RawValue_ByHandle(g_fSpotInnerCone, &m_LightDesc.fInnerCone, sizeof(_float));
            _shader->Bind_RawValue_ByHandle(g_fSpotOuterCone, &m_LightDesc.fOuterCone, sizeof(_float));
            passIndex = _UINT(LIGHT::SPOTLIGHT);
            break;
        default:
            break;
    }

    _shader->Bind_RawValue_ByHandle(g_vLightDiffuse, &m_LightDesc.vDiffuse, sizeof(_float4));
    _shader->Bind_RawValue_ByHandle(g_vLightAmbient, &m_LightDesc.vAmbient, sizeof(_float4));
    _shader->Bind_RawValue_ByHandle(g_vLightSpecular, &m_LightDesc.vSpecular, sizeof(_float4));

    _shader->Begin(passIndex);
    _shader->Bind_Resources(passIndex);

    _buffer->Bind_Resource();
    _buffer->Render(fTimeDelta);

    return S_OK;
}

//거리계산해서 가지고 있는다
void Engine::Light::Update_LightPlayer_Distance(_vector _vPlayerPos)
{
    _vector vLightPos = XMLoadFloat4(&m_LightDesc.vPosition);
    _vector vDir = vLightPos - _vPlayerPos;
   _vector Distance = XMVector3Length(vDir);
   m_LightDesc.fPlayerDistance = XMVectorGetX(Distance);
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Light* Engine::Light::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& _lightdesc)
{
	Light* pInstance = new Light(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(_lightdesc), L"Light 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Light::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
/******************************************************* 객체 반환 함수 *******************************************************/

