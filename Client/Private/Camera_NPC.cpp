#include "Client_Define.h"
#include "Camera_NPC.h"
#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Camera_NPC::Camera_NPC()
{
}

Camera_NPC::Camera_NPC(ID3D11Device* _device, ID3D11DeviceContext* _context)
    : Camera(_device, _context)
{
}

Camera_NPC::Camera_NPC(const Camera_NPC& _original)
    : Camera(_original)
{
}

Camera_NPC::~Camera_NPC()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Camera_NPC::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);
    return S_OK;
}

HRESULT Camera_NPC::Initialize(void* _arg)
{
    static _uint namenum = 0;

    CAMERA_DESC desc = {};
    if (_arg == nullptr)
    {
        desc.vEye = _float3(1.f, 1.f, 1.f);
        desc.vAt = _float3(0.f, 0.f, 1.f);
        desc.fFov = XMConvertToRadians(45.f);
        desc.fNear = 0.1f;
        desc.fFar = 50.f;
        desc.fRotationSpeed = XMConvertToRadians(180.f);
        desc.fAspect = _float(WINCX) / WINCY;
        _arg = &desc;
    }

    CAST(CAMERA_DESC*)(_arg)->wstrName = L"Camera_NPC_" + to_wstring(namenum++);

    CHECK_FAILED(Camera::Initialize(_arg), E_FAIL);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Camera_NPC::Update_Priority(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Camera_NPC::Update(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 메인 카메라 업데이트 함수 ////////////////////////////////////////////////////////
void Camera_NPC::Update_Camera(const _float fTimeDelta)
{
    if (m_bIsActive == false)
        return;

    // 카메라 전환 시 이전 카메라 행렬로 초기화
    if (m_bIsChanged)
    {
        m_pTransformCom->Set_Matrix(m_pGameInstance->Get_PrevCamera_Matrix());
        m_bIsChanged = false;
    }

    if (!m_bTransitioning)
    {
        // 전환 끝나면 목표 위치에 고정
        Bind_PipeLine();
        return;
    }

    m_fElapsed += fTimeDelta;
    _float rawT = clamp(m_fElapsed / m_fDuration, 0.f, 1.f);
    _float t = EaseInOutCubic(rawT);

    // 위치: Cubic Bezier 보간
    _vector vPosition = Compute_CubicBezier(t);
    m_pTransformCom->Set_State(DIRECTION::POSITION, vPosition);

    // 회전: Quaternion Slerp
    _vector vStartQ = XMLoadFloat4(&m_vStartQuat);
    _vector vEndQ = XMLoadFloat4(&m_vEndQuat);
    _vector vCurrentQ = XMQuaternionSlerp(vStartQ, vEndQ, t);

    // Quaternion -> 회전 행렬 -> 축 추출
    _matrix matRot = XMMatrixRotationQuaternion(vCurrentQ);
    m_pTransformCom->Set_State(DIRECTION::RIGHT, XMVector3Normalize(matRot.r[0]));
    m_pTransformCom->Set_State(DIRECTION::UP, XMVector3Normalize(matRot.r[1]));
    m_pTransformCom->Set_State(DIRECTION::LOOK, XMVector3Normalize(matRot.r[2]));

    // 전환 완료 판정
    if (rawT >= 1.f)
    {
        m_bTransitioning = false;
        m_bTransitionDone = true;
    }

    Bind_PipeLine();
    m_pGameInstance->Add_RenderObject(RENDER_GROUP::CAMERA, this);
}
/******************************************************* 메인 카메라 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Camera_NPC::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Camera_NPC::Render(const _float fTimeDelta)
{
    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 전환 시작 함수 ////////////////////////////////////////////////////////
void Camera_NPC::Start_Transition(const TRANSITION_DESC& _desc)
{
    m_bTransitioning = true;
    m_bTransitionDone = false;
    m_fElapsed = 0.f;
    m_fDuration = _desc.fDuration;
    m_fArcHeight = _desc.fArcHeight;
    // 행렬을 이전 카메라 위치로 초기화
    m_pTransformCom->Set_Matrix(m_pGameInstance->Get_PrevCamera_Matrix());

    // P0, P1, P2, P3 점으로 스플라인 만들어서 곡선 타고 이동

    // P0: 현재 카메라 위치
    XMStoreFloat4(&m_vP0, m_pTransformCom->Get_State(DIRECTION::POSITION));

    // P3: 목표 위치
    m_vP3 = _desc.vTargetPosition;

    // 도착 후 바라볼 지점
    m_vTargetLookAt = _desc.vTargetLookAt;

    // 시작 회전: 현재 카메라 행렬에서 Quaternion 추출
    _matrix matCurrent = m_pTransformCom->Get_WorldMatrix();
    // 스케일 제거 후 회전만 추출
    _vector vScale, vRot, vTrans;
    XMMatrixDecompose(&vScale, &vRot, &vTrans, matCurrent);
    XMStoreFloat4(&m_vStartQuat, vRot);

    // 목표 회전: 목표 위치에서 LookAt 지점을 바라보는 Quaternion
    _vector vEndRot = LookAt_Quaternion(
        XMLoadFloat4(&m_vP3),
        XMLoadFloat4(&m_vTargetLookAt),
        XMVectorSet(0.f, 1.f, 0.f, 0.f));
    XMStoreFloat4(&m_vEndQuat, vEndRot);

    // Bezier 제어점 계산
    Compute_ControlPoints();
}
/******************************************************* 전환 시작 함수 *******************************************************/



//////////////////////////////////////////////////////// Bezier 제어점 계산 함수 ////////////////////////////////////////////////////////
void Camera_NPC::Compute_ControlPoints()
{
    _vector vStart = XMLoadFloat4(&m_vP0);
    _vector vEnd = XMLoadFloat4(&m_vP3);

    // 시작 Look 방향 (현재 카메라가 바라보는 방향으로 밀어냄)
    //_vector vStartLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));
    
    _vector vToEnd = XMVector3Normalize(XMVectorSubtract(vEnd, vStart));

    // 목표 Look 방향 (도착점에서 NPC를 바라보는 방향의 반대 = 뒤쪽에서 진입)
    _vector vEndLook = XMVector3Normalize(
        XMVectorSubtract(XMLoadFloat4(&m_vTargetLookAt), vEnd));

    // P0~P3 사이 거리의 비율로 제어점 밀어내기
    _float totalDist = XMVectorGetX(XMVector3Length(XMVectorSubtract(vEnd, vStart)));
    _float pushDist = totalDist * m_fArcHeight * 0.5f;

    // P1: 시작점에서 Look 방향으로 밀어냄
    //_vector vP1 = XMVectorAdd(vStart, XMVectorScale(vStartLook, pushDist));
    _vector vP1 = XMVectorAdd(vStart, XMVectorScale(vToEnd, pushDist));
    XMStoreFloat4(&m_vP1, vP1);

    // P2: 끝점에서 Look 반대 방향으로 밀어냄 (뒤에서 접근하는 느낌)
    // 반대 방향이라 네거티브
    _vector vP2 = XMVectorSubtract(vEnd, XMVectorScale(vEndLook, pushDist));
    XMStoreFloat4(&m_vP2, vP2);
}
/******************************************************* Bezier 제어점 계산 함수 *******************************************************/



//////////////////////////////////////////////////////// Cubic Bezier 보간 함수 ////////////////////////////////////////////////////////
_vector Camera_NPC::Compute_CubicBezier(_float _t)
{
    // B(t) = (1-t)^3 * P0 + 3(1-t)^2 * t * P1 + 3(1-t) * t^2 * P2 + t^3 * P3
    _float oneMinusT = 1.f - _t;
    _float oneMinusT2 = oneMinusT * oneMinusT;
    _float oneMinusT3 = oneMinusT2 * oneMinusT;
    _float t2 = _t * _t;
    _float t3 = t2 * _t;

    _vector vP0 = XMLoadFloat4(&m_vP0);
    _vector vP1 = XMLoadFloat4(&m_vP1);
    _vector vP2 = XMLoadFloat4(&m_vP2);
    _vector vP3 = XMLoadFloat4(&m_vP3);

    _vector vResult = XMVectorScale(vP0, oneMinusT3);
    vResult = XMVectorAdd(vResult, XMVectorScale(vP1, 3.f * oneMinusT2 * _t));
    vResult = XMVectorAdd(vResult, XMVectorScale(vP2, 3.f * oneMinusT * t2));
    vResult = XMVectorAdd(vResult, XMVectorScale(vP3, t3));

    return vResult;
}
/******************************************************* Cubic Bezier 보간 함수 *******************************************************/



//////////////////////////////////////////////////////// LookAt Quaternion 계산 함수 ////////////////////////////////////////////////////////
_vector Camera_NPC::LookAt_Quaternion(_fvector _eye, _fvector _target, _fvector _worldUp)
{
    // View 행렬의 역행렬로 월드 회전 Quaternion 추출
    _matrix matView = XMMatrixLookAtLH(_eye, _target, _worldUp);
    _matrix matWorld = XMMatrixInverse(nullptr, matView);

    _vector vScale, vRot, vTrans;
    XMMatrixDecompose(&vScale, &vRot, &vTrans, matWorld);

    return vRot;
}
/******************************************************* LookAt Quaternion 계산 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Camera_NPC* Camera_NPC::Create(ID3D11Device* _device, ID3D11DeviceContext* _context, LEVEL _level)
{
    Camera_NPC* pInstance = new Camera_NPC(_device, _context);
    MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Camera_NPC 원본 생성 실패", L"경고!!!", nullptr);
    return pInstance;
}

GameObject* Camera_NPC::Clone(void* _arg)
{
    Camera_NPC* pInstance = new Camera_NPC(*this);
    MSG_FAIL(pInstance->Initialize(_arg), L"Camera_NPC 복사 실패", L"경고!!!", nullptr);
    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Camera_NPC::Free()
{
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/