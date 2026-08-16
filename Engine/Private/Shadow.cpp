#include "Engine_Define.h"
#include "Shadow.h"
#include "GameInstance.h"
//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Shadow::Shadow()
{
}

Engine::Shadow::Shadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

Engine::Shadow::~Shadow()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 조명 관리 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Shadow::Add_ShadowLight(const SHADOW_DESC& _shadowDesc)
{
    m_tShadowDesc = _shadowDesc;
    _vector _at = XMLoadFloat4(&m_tShadowDesc.vAt);
    _vector _position = XMLoadFloat4(&m_tShadowDesc.vPosition);
    XMStoreFloat4(&m_tShadowDesc.vPosition,
        _position + _at);
    m_tShadowDesc.vPosition.w = 1.f;

    XMMATRIX lightView = XMMatrixLookAtLH(
        XMLoadFloat4(&m_tShadowDesc.vPosition),
        XMLoadFloat4(&m_tShadowDesc.vAt),
        XMVectorSet(0.f, 1.f, 0.f, 0.f));
    XMStoreFloat4x4(&m_LightMarices[_UINT(D3DTS_VIEW)], lightView);

    XMVECTOR atLightSpace = XMVector4Transform(
        XMVectorSetW(_at, 1.f), lightView);
    _float centerZ = XMVectorGetZ(atLightSpace);

    _float orthoSize = m_tShadowDesc.vOrthSize;
    _float depthRange = m_tShadowDesc.fDepth;

    //x/y는 OrthographicLH(원점 중심), near/far만 centerZ 기준
    XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(
        -orthoSize * 0.5f, orthoSize * 0.5f,   //x는 원점 중심 (LookAt이 잡아줌)
        -orthoSize * 0.5f, orthoSize * 0.5f,   //y도 원점 중심
        centerZ - depthRange, centerZ + depthRange); 

    _float shadowMapSize = 8192.f;
    _float texelSize = orthoSize / shadowMapSize;

    XMMATRIX lightViewProj = XMMatrixMultiply(lightView, lightProj);

    XMVECTOR origin = XMVector4Transform(XMVectorZero(), lightViewProj);
    _float snapX = fmodf(XMVectorGetX(origin), texelSize);
    _float snapY = fmodf(XMVectorGetY(origin), texelSize);

    XMMATRIX snapOffset = XMMatrixTranslation(-snapX, -snapY, 0.f);
    XMMATRIX Result = XMMatrixMultiply(lightViewProj, snapOffset);

    XMStoreFloat4x4(&m_LightMarices[_UINT(D3DTS_VIEWPROJ)], Result);

    //여기서 라이트 방향 만든다
    _vector LightDir = XMVector3Normalize(XMLoadFloat4(&m_tShadowDesc.vAt) - XMLoadFloat4(&m_tShadowDesc.vPosition));
    XMStoreFloat4(&m_vLightDir, LightDir);
    return S_OK;
}

void Engine::Shadow::ResizeBuffers(_float _width, _float _height)
{
    XMStoreFloat4x4(&m_LightMarices[_UINT(D3DTS_PROJ)],
        XMMatrixPerspectiveFovLH(
            m_tShadowDesc.fFovy,
            _width / _height,
            m_tShadowDesc.fNearZ,
            m_tShadowDesc.fFarZ));
}

HRESULT Engine::Shadow::Add_ShadowCascadeLight(_int _iNumCascade, const SHADOW_DESC& _shadowDesc, _float _Bias[])
{
    m_iNumCascade = _iNumCascade;

    //케스케이드 분할점 계산
    m_OutSplits[0] = _shadowDesc.fNearZ;
    m_tShadowCascadeDesces[0].fDepth = _shadowDesc.fDepth;
    float lambda = 0.85f;  // 0.5~0.85 사이에서 장면에 맞게 튜닝
    for (int i = 1; i <= m_iNumCascade; i++)
    {
        float p = static_cast<float>(i) / (m_iNumCascade );

        float log = _shadowDesc.fNearZ * powf(_shadowDesc.fFarZ / _shadowDesc.fNearZ, p);
        float uni = _shadowDesc.fNearZ + (_shadowDesc.fFarZ - _shadowDesc.fNearZ) * p;
        float split = lambda * log + (1.0f - lambda) * uni;

        m_OutSplits[i] = split;
    }
    //계산된 분할점 가지고 매트릭스 만들기
    //ratio높혀서 더섞으니까 되긴함
    float ratio = 0.7f;
    for (int i = 0; i < m_iNumCascade; i++)
    {
        //여기서 오버랩 적용하기
        // AI: 오버랩을 포함한 실제 near/far 계산
        float baseNear = m_OutSplits[i];
        float baseFar = m_OutSplits[i + 1];

        // AI: 첫 번째 cascade는 near를 0으로
        float actualNear = (i == 0) ? 0.0f : baseNear * (1.0f - ratio);

        // AI: 마지막 cascade는 far를 그대로, 아니면 확장
        float actualFar = (i == m_iNumCascade - 1) ? baseFar : baseFar * (1.0f + ratio);

        m_tShadowCascadeDesces[i].fNearZ = baseNear;
        m_tShadowCascadeDesces[i].fFarZ = baseFar;
        m_tShadowCascadeDesces[i].vPosition = _shadowDesc.vPosition;
        m_tShadowCascadeDesces[i].vAt = _shadowDesc.vAt;
        m_tShadowCascadeDesces[i].vOrthSize = _shadowDesc.vOrthSize * powf(2.0f, (_float)i);

        m_bias[i] = _Bias[i];
    } 
   
    Split.x = m_OutSplits[1];
    Split.y = m_OutSplits[2];
    Split.z = m_OutSplits[3];
    Split.w = m_OutSplits[4];
    if (_shadowDesc.vAddPos.x == 0.f && _shadowDesc.vAddPos.y == 0.f)
    {
        m_vAddPos = _float4{ 0.f, 20.f, -20.f, 0.f };
    }
    else
        m_vAddPos = _shadowDesc.vAddPos;

    Follow_ShadowCascade_Matrix(XMLoadFloat4(&_shadowDesc.vAt));

    return S_OK;
}

//카메라 Frustrum 기반 Ortho 자동계산
void Engine::Shadow::Follow_ShadowCascade_Matrix(_vector _at)
{
    for (int i = 0; i < m_iNumCascade; i++)
    {
        XMStoreFloat4(&m_tShadowCascadeDesces[i].vAt, _at);
        XMStoreFloat4(&m_tShadowCascadeDesces[i].vPosition,
            XMVectorSet(m_vAddPos.x, m_vAddPos.y, m_vAddPos.z, 0.f) + _at);
        m_tShadowCascadeDesces[i].vPosition.w = 1.f;

        XMMATRIX lightView = XMMatrixLookAtLH(
            XMLoadFloat4(&m_tShadowCascadeDesces[i].vPosition),
            XMLoadFloat4(&m_tShadowCascadeDesces[i].vAt),
            XMVectorSet(0.f, 1.f, 0.f, 0.f));
        XMStoreFloat4x4(&m_LightCascadeMarices[_UINT(D3DTS_VIEW)][i], lightView);

        //플레이어의 light space z를 구해서 near/far 중심으로 사용
        XMVECTOR atLightSpace = XMVector4Transform(
            XMVectorSetW(_at, 1.f), lightView);
        float centerZ = XMVectorGetZ(atLightSpace);

        float orthoSize = m_tShadowCascadeDesces[i].vOrthSize;
        float depthRange = m_tShadowCascadeDesces[0].fDepth;

        //x/y는 OrthographicLH(원점 중심), near/far만 centerZ 기준
        XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(
            -orthoSize * 0.5f, orthoSize * 0.5f,   //x는 원점 중심 (LookAt이 잡아줌)
            -orthoSize * 0.5f, orthoSize * 0.5f,   //y도 원점 중심
            centerZ - depthRange, centerZ + depthRange);  //z만 플레이어 중심!

        XMStoreFloat4x4(&m_LightCascadeMarices[_UINT(D3DTS_PROJ)][i], lightProj);

        XMMATRIX Result = XMMatrixMultiply(lightView, lightProj);
        XMStoreFloat4x4(&m_LightCascadeMarices[_UINT(D3DTS_VIEWPROJ)][i], Result);
    }

    return;
}
/******************************************************* 조명 관리 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Shadow* Engine::Shadow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new Shadow(pDevice, pContext);
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Shadow::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
/******************************************************* 객체 반환 함수 *******************************************************/

