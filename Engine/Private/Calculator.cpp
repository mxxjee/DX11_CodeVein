#include "Engine_Define.h"
#include "Calculator.h"

#include <random>


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Calculator::Calculator()
{
}

Engine::Calculator::Calculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

Engine::Calculator::~Calculator()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Calculator::Initialize()
{
    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 계산기 ////////////////////////////////////////////////////////
_float Engine::Calculator::RandomValue(_float _min, _float _max)
{
    thread_local mt19937 gen(random_device{}());
    // distribution도 thread_local로 캐싱
    thread_local uniform_real_distribution<_float> dist(0.f, 1.f);

    // [0,1] 범위로 뽑은 뒤 스케일
    return _min + dist(gen) * (_max - _min);
}

_uint Engine::Calculator::RandomValue_int(_uint _min, _uint _max)
{
    thread_local random_device rd;
    thread_local mt19937 gen(rd());

    uniform_int_distribution<_uint> dist(_min, _max);
    return dist(gen);
}

_vector Engine::Calculator::QuaternionToDegrees(_fvector _quaternion)
{
    // 쿼터니언 -> 행렬
    _matrix rotation = XMMatrixRotationQuaternion(_quaternion);

    // 행렬 -> 라디안 (YXZ 오일러 순서)
    _float sinPitch = -rotation.r[2].m128_f32[1];
    sinPitch = max(-1.f, min(1.f, sinPitch));  // NaN 방지 클램핑
    _float pitch = asinf(sinPitch);

    _float3 eulerRadian = {};
    const _float epsilon = 0.001f; // 짐벌락 방지용
    _float cosPitch = cosf(pitch);

    if (fabsf(cosPitch) > epsilon)  // 절댓값으로 양쪽 짐벌락 체크
    {
        eulerRadian.x = pitch;
        eulerRadian.y = atan2f(rotation.r[2].m128_f32[0], rotation.r[2].m128_f32[2]);
        eulerRadian.z = atan2f(rotation.r[0].m128_f32[1], rotation.r[1].m128_f32[1]);
    }
    else
    {
        eulerRadian.x = pitch;
        eulerRadian.y = 0.f;
        eulerRadian.z = atan2f(-rotation.r[1].m128_f32[0], rotation.r[0].m128_f32[0]);
    }

    return XMVectorMultiply(XMLoadFloat3(&eulerRadian), XMVectorReplicate(180.0f / XM_PI));
}
/******************************************************* 계산기 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Calculator* Engine::Calculator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    Calculator* pInstance = new Calculator(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(), L"Calculator 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Calculator::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
/******************************************************* 객체 반환 함수 *******************************************************/

