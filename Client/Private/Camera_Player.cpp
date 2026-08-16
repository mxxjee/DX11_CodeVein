#include "Client_Define.h"
#include "Camera_Player.h"
#include "GameInstance.h"
#include "PhysX_Function.h"
#include "UIObj_LockOn.h"
#include "Monster.h"
#include "Player.h"
#include "Yakumo.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Camera_Player::Camera_Player()
{
}

Camera_Player::Camera_Player(ID3D11Device* _device, ID3D11DeviceContext* _context)
    : Camera(_device, _context)
{
}

Camera_Player::Camera_Player(const Camera_Player& _original)
    : Camera(_original)
{
}

Camera_Player::~Camera_Player()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Camera_Player::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);
    return S_OK;
}

HRESULT Camera_Player::Initialize(void* _arg)
{
    static _uint namenum = 0;

    CAMOBJ_DESC desc = {};
    //if (_arg == nullptr)
    {

        desc.vEye = _float3(0.f, 10.f, -10.f);
        desc.vAt = _float3(0.f, 0.f, 1.f);
        desc.fFov = XMConvertToRadians(55.f);
        desc.fNear = 0.1f;
        desc.fFar = 500.f;
        desc.fSensor = 0.1f;        // <<< 이거 바꾸면 카메라 감도 바뀜(낮으면 느려지고 높으면 빨라진다)
        desc.fSpeed = 15.f;         // 얘는 안 씀
        desc.fRotationSpeed = XMConvertToRadians(180.f);
        desc.fAspect = _float(WINCX) / WINCY;
        desc.fSmoothness = 19.f;    // 얘도 안 씀
        
        if (_arg != nullptr)
        {
            CAMOBJ_DESC* pDesc = CAST(CAMOBJ_DESC*)(_arg);
            if (pDesc->fFar >= 1.f) desc.fFar = pDesc->fFar;
        }
        
        _arg = &desc;
    }

    CAST(GAMEOBJECT_DESC*)(_arg)->wstrName = L"Camera_Player" + namenum++;
    CHECK_FAILED(Camera::Initialize(_arg), E_FAIL);

    CHECK_FAILED(Ready_Value(_arg), E_FAIL);

    // 이벤트로 락온 해도 되고
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<CameraEvent>([this](const CameraEvent& _event)
        {
            Event_Set(_event);
        }));


    //ui창 띄울시 입력막기
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<INPUT_LOCK_EVENT>([this](const INPUT_LOCK_EVENT& _event)
        {
            m_bStatic = _event.bLock;
            if (!m_bStatic && m_eCameraState != CAMERA_STATE::CINEMATIC &&m_eCameraState!=CAMERA_STATE::LOCK_ON)
            {
                m_eCameraState = CAMERA_STATE::FREE_FOLLOW;
            }
        }));


    return S_OK;
}

HRESULT Client::Camera_Player::Ready_Value(void* _arg)
{
    m_fSmoothness = static_cast<CAMOBJ_DESC*>(_arg)->fSmoothness;
    m_fSensor = static_cast<CAMOBJ_DESC*>(_arg)->fSensor;
    m_fMinPitch = XMConvertToRadians(-70.f);
    m_fMaxPitch = XMConvertToRadians(80.f);
    m_fDistance = 3.f;
    m_fCurrentDistance = m_fDistance;

    m_fBaseFov = m_fFov;

    /* 카메라가 얼마나 높이 있을지 결정 */
    m_fPivotHeight = 1.5f;
    m_fRockOnPivotHeight = 0.3f;

    /* 락온 */
    m_fZoomOutSpeed = 10.f;
    m_fZoomInSpeed = 10.f;
    m_fLockOnPitchNear = 0.2f;   // 가까울 때 Pitch (살짝 위에서)
    m_fLockOnPitchFar = 0.05f;    // 멀 때 Pitch (거의 수평)
    m_fLockOnPitchThreshold = 2.f;   // 거리 이하부터 Pitch 변화 시작
    m_fLockOnDistance = 3.f;    // 락온했을때 얼마나 멀어져있을지
    m_fLockOnYOffsetMax = 0.3f; // 락온시 최대 높이 상승량
    m_fLockOnYOffsetStartDist = 1.0f;   // 어디서부터 높이 올릴지
    m_fLockOnYOffsetEndDist = 5.0f;     // 어디서부터 높이 올릴지


    /* 락온 전환 보간 */
    m_bLockOnBlending = false;
    m_fLockOnBlendSpeed = 4.f;               // 전환 속도 (높을수록 빠름)
    m_fLockOnBlendThreshold = 0.01f;         // 라디안 차이 이하면 보간 완료

    m_fLookAtFollowSpeed = 3.5f; // LookAt 추종 속도 (낮을수록 느릿느릿)
    m_fLookAtMouseSpeed = m_fLookAtFollowSpeed * 1.5f;

    m_bLookAtLockInitialized = false;
    m_bLookAtFreeInitialized = false;

    m_fReturnDuration = 0.2f;

    return S_OK;
}

void Client::Camera_Player::Event_Set(const CameraEvent& _event)
{
    // 카메라 액션 처리
    if (_event.eCameraAction == CAMERA_ACTION::SHAKE)
    {
        // 우선순위가 현재 이상일 때만 적용
        if (static_cast<_uint>(_event.tShake.ePriority) >= static_cast<_uint>(m_tShake.ePriority))
        {
            Start_Shake(_event.tShake, XMLoadFloat4(&_event.tShake.vKickDirection));
        }
    }

    // 상태 변경 이벤트가 아니면 조기 종료
    if (_event.eCameraState == CAMERA_STATE::END)
        return;

    // 시네마틱 중일 때 m_ePrevState가 CINEMATIC으로 오염되는 것 방지
    if (m_eCameraState != CAMERA_STATE::CINEMATIC && m_eCameraState != _event.eCameraState)
    {
        m_ePrevState = m_eCameraState;
    }

    m_eCameraState = _event.eCameraState;

    // 상태별 진입 로직 정리
    if (m_eCameraState == CAMERA_STATE::LOCK_ON)
    {
        if (_event.pLockOnTarget != nullptr)
            Set_LockOnTarget(_event.pLockOnTarget);
        else
            Clear_LockOnTarget();
    }
    else if (m_eCameraState == CAMERA_STATE::CINEMATIC)
    {
        if (!_event.vecKeyframes.empty())
            Start_Cinematic(_event.vecKeyframes, _event.bCinematicAutoReturn);
    }
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Camera_Player::Update_Priority(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Camera_Player::Update_Parallel(const _float fTimeDelta)
{
    if (m_pYakumo == nullptr)
    {
        m_pYakumo = CAST(Yakumo*)(m_pGameInstance->Get_Companion());
    }

    //조명 따라다니기 + 각도
    LIGHT_DESC* temp = m_pGameInstance->Get_LightDesc(2);

    temp->vPosition = m_pTransformCom->Get_Position_Float4();

    //그냥 look을 normalize
    _vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));
    _vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::RIGHT));

    _matrix rotMat = XMMatrixRotationAxis(vRight, XMConvertToRadians(15.f)); //Right를 15도 회전(회전행렬만들고)
    _vector dir = XMVector3Normalize(XMVector3TransformNormal(vLook, rotMat)); //벡터 * 행렬(15도 회전행렬을 곱해준다)
    XMStoreFloat4(&temp->vDirection, dir);

    m_pGameInstance->Set_LightDesc(2, *temp);

    return 0;
}

_int Camera_Player::Update(const _float fTimeDelta)
{

    return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Camera_Player::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Camera_Player::Render(const _float fTimeDelta)
{
    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 메인 카메라 업데이트 함수 ////////////////////////////////////////////////////////
void Camera_Player::Update_Camera(const _float fTimeDelta)
{
    if (!m_bIsActive)
        return;

    // 카메라 전환 시 이전 카메라 행렬로 초기화
    if (m_bIsChanged)
    {
        m_pTransformCom->Set_Matrix(m_pGameInstance->Get_PrevCamera_Matrix());
        m_bIsChanged = false;
    }

    if (m_pTarget == nullptr)
    {
        m_pTarget = m_pGameInstance->Get_Player();
        if (m_pTarget)
        {
            Safe_AddRef(m_pTarget);
        }
        else
            return;
    }

#ifdef _DEBUG // 카메라 속도 / FOV값 테스트용
    if (m_pGameInstance->KeyPress(DIK_LCONTROL))
    {
        if (m_pGameInstance->KeyDown(DIK_NUMPADMINUS))
        {
            m_fFov = XMConvertToRadians(XMConvertToDegrees(m_fFov) - 1);
            COUT(XMConvertToDegrees(m_fFov));
        }
        else if (m_pGameInstance->KeyDown(DIK_NUMPADPLUS))
        {
            m_fFov = XMConvertToRadians(XMConvertToDegrees(m_fFov) + 1);
            COUT(m_fLookAtFollowSpeed);
            COUT(XMConvertToDegrees(m_fFov));
        }
    }
    else
    {
        if (m_pGameInstance->KeyDown(DIK_NUMPADMINUS))
        {
            m_fLookAtFollowSpeed = m_fLookAtFollowSpeed - 0.1f;
            COUT(m_fLookAtFollowSpeed);
        }
        else if (m_pGameInstance->KeyDown(DIK_NUMPADPLUS))
        {
            m_fLookAtFollowSpeed = m_fLookAtFollowSpeed + 0.1f;
            COUT(m_fLookAtFollowSpeed);
        }
    }
#endif // _DEBUG


    // 시네마틱 업데이트가 가장 우선
    if (m_eCameraState == CAMERA_STATE::CINEMATIC)
    {
        Compute_Cinematic(fTimeDelta);
    }
    // 락온 상태가 아닐 때만 마우스 입력 적용
    else if (m_eCameraState == CAMERA_STATE::LOCK_ON && m_pLockOnTarget)
    {
        Compute_LockOn(fTimeDelta);
    }
    else
    {
        Process_MouseInput(fTimeDelta);
        Compute_FreeFollow(fTimeDelta);
    }

    // 위치/회전 적용
    m_pTransformCom->Set_State(DIRECTION::POSITION, m_vLogicalPosition);

    _matrix matRotation = XMMatrixRotationRollPitchYaw(m_fPitch, m_fYaw, 0.f);
    m_pTransformCom->Set_State(DIRECTION::RIGHT, XMVector3Normalize(matRotation.r[0]));
    m_pTransformCom->Set_State(DIRECTION::UP, XMVector3Normalize(matRotation.r[1]));
    m_pTransformCom->Set_State(DIRECTION::LOOK, XMVector3Normalize(matRotation.r[2]));

    Update_Shake(fTimeDelta);

    Bind_PipeLine();
    m_pGameInstance->Add_RenderObject(RENDER_GROUP::CAMERA, this);
}
/******************************************************* 메인 카메라 업데이트 함수*******************************************************/



//////////////////////////////////////////////////////// 마우스 입력 함수 ////////////////////////////////////////////////////////
void Camera_Player::Process_MouseInput(const _float fTimeDelta)
{
    CHECK_TRUE(m_bStatic);

    m_bMouseMoved = false;

    _long _mouseMove = {};

    if ((_mouseMove = m_pGameInstance->MouseMove(MOUSEMOVESTATE::X)))
    {
        m_fYaw += m_fSensor * _mouseMove * fTimeDelta;
        m_bMouseMoved = true;
    }

    if ((_mouseMove = m_pGameInstance->MouseMove(MOUSEMOVESTATE::Y)))
    {
        m_fPitch += m_fSensor * _mouseMove * fTimeDelta;
        m_bMouseMoved = true;
    }

    m_fPitch = clamp(m_fPitch, m_fMinPitch, m_fMaxPitch);

    // Yaw 정규화
    while (m_fYaw > XM_PI)  m_fYaw -= XM_2PI;
    while (m_fYaw < -XM_PI) m_fYaw += XM_2PI;
}
/******************************************************* 마우스 입력 함수*******************************************************/



//////////////////////////////////////////////////////// 프리 팔로우 계산 함수 ////////////////////////////////////////////////////////
void Client::Camera_Player::Compute_FreeFollow(const _float fTimeDelta)
{
    Player* pPlayer = CAST(Player*)(m_pTarget);
    _vector vRenderPos = pPlayer->Get_RenderPos();

    // 실제 캐릭터 피봇 (카메라 궤도/벽충돌 계산용)
    _vector vActualPivot = XMVectorAdd(vRenderPos, XMVectorSet(0.f, m_fPivotHeight, 0.f, 0.f));
    vActualPivot = XMVectorSetW(vActualPivot, 1.f);

    // 누적된 Pitch와 Yaw로 카메라 위치 계산
    _matrix matRotation = XMMatrixRotationRollPitchYaw(m_fPitch, m_fYaw, 0.f);
    _vector direction = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, -1.f, 0.f), matRotation);

    // 벽 충돌은 실제 캐릭터 피봇 기준
    _float fAllowedDistance = Compute_WallDistance(vActualPivot, direction, m_fDistance);
    Update_Distance(fTimeDelta, fAllowedDistance);

    // LookAt 보간 (가상 피봇)
    if (!m_bLookAtFreeInitialized)
    {
        XMStoreFloat4(&m_vSmoothLookAtFree, vActualPivot);
        m_bLookAtFreeInitialized = true;
    }
    else
    {
        // 이번 프레임에 마우스 입력이 있었는지
        _float fSpeed = m_bMouseMoved ? m_fLookAtMouseSpeed : m_fLookAtFollowSpeed;
        _float fRatio = 1.0f - expf(-fSpeed * fTimeDelta);
        _vector vSmooth = XMVectorLerp(XMLoadFloat4(&m_vSmoothLookAtFree), vActualPivot, fRatio);
        XMStoreFloat4(&m_vSmoothLookAtFree, vSmooth);
    }

    // 카메라 위치는 보간 피봇 기준, 거리는 실제 피봇 기준
    // FMA로 곱셈+덧셈 한 명령어 처리
    _vector vSmoothLookAt = XMLoadFloat4(&m_vSmoothLookAtFree);
    _vector vPosition = XMVectorMultiplyAdd(direction, XMVectorReplicate(fAllowedDistance), vSmoothLookAt);
    XMStoreFloat4(&m_vLogicalPosition, vPosition);
}
/******************************************************* 프리 팔로우 계산 함수 *******************************************************/




//////////////////////////////////////////////////////// 락온 함수 ////////////////////////////////////////////////////////
void Client::Camera_Player::Set_LockOnTarget(GameObject* _target)
{
    if (m_pLockOnTarget != nullptr)
        Safe_Release(m_pLockOnTarget);

    m_pLockOnTarget = dynamic_cast<Monster*>(_target);
    CHECK_JUST_NULL(m_pLockOnTarget);

    Safe_AddRef(m_pLockOnTarget);
    

    m_eCameraState = CAMERA_STATE::LOCK_ON;
    m_bLockOnBlending = true; // 전환 보간 시작

    // 락온 피봇을 현재 프리팔로우 피봇으로 스냅 (전환 시 튀지 않게)
    m_vSmoothLookAtLock = m_vSmoothLookAtFree;
    m_bLookAtLockInitialized = true;

    //UI이벤트
    UIObj_LockOn::LockOnUIEvent LockOnEvent;
    LockOnEvent.bActive = true;
    LockOnEvent.eType = UIObj_LockOn::LOCKON;
    LockOnEvent.pTarget = m_pLockOnTarget;
    m_pGameInstance->Publish(LockOnEvent);
}

void Client::Camera_Player::Clear_LockOnTarget()
{
    // 프리팔로우 피봇을 현재 락온 피봇으로 스냅 (해제 시 튀지 않게)
    m_vSmoothLookAtFree = m_vSmoothLookAtLock;
    m_bLookAtFreeInitialized = true;
    m_bLookAtLockInitialized = false;

    m_fCurrentDistance = min(m_fCurrentDistance, m_fDistance);

    // UI이벤트
    UIObj_LockOn::LockOnUIEvent LockOnEvent;
    LockOnEvent.bActive = true;
    LockOnEvent.eType = UIObj_LockOn::CLEAR;

    m_pGameInstance->Publish(LockOnEvent);
    m_eCameraState = CAMERA_STATE::FREE_FOLLOW;
}

void Camera_Player::Toggle_LockOn()
{
    if (m_eCameraState == CAMERA_STATE::LOCK_ON)
    {
        m_eCameraState = CAMERA_STATE::FREE_FOLLOW;
        m_pLockOnTarget = nullptr;
    }
    else
    {
        if (m_pLockOnTarget)
            m_eCameraState = CAMERA_STATE::LOCK_ON;
    }
}

void Client::Camera_Player::Compute_LockOn(const _float fTimeDelta)
{
    if (!m_pLockOnTarget)
    {
        m_eCameraState = CAMERA_STATE::FREE_FOLLOW;
        return;
    }

    Player* pPlayer = DCAST(Player*)(m_pTarget);
    _vector playerPos = pPlayer->Get_RenderPos();

    // 카메라 방향 계산(플레이어 뒤에서 적 위치쪽으로 볼 수 있게)
    _vector enemyPos = m_pLockOnTarget->Get_Position();
    _vector toEnemy = XMVectorSetY(XMVectorSubtract(enemyPos, playerPos), 0.f);

    _float fDist = XMVectorGetX(XMVector3Length(toEnemy)); //플레이어와 거리 대상 거리 가져오기 

    // 거리 기반 Y오프셋 계산
    _float yOffset = m_fLockOnYOffsetMax;
    if (fDist > m_fLockOnYOffsetStartDist)
    {
        _float t = clamp(
            (fDist - m_fLockOnYOffsetStartDist) / (m_fLockOnYOffsetEndDist - m_fLockOnYOffsetStartDist),
            0.f, 1.f);
        yOffset = m_fLockOnYOffsetMax * (1.f - t);
    }

    // pivot에 오프셋 적용
    _vector pivot = XMVectorAdd(playerPos, XMVectorSet(0.f, m_fPivotHeight + m_fRockOnPivotHeight + yOffset, 0.f, 0.f));
    pivot = XMVectorSetW(pivot, 1.f);

    _vector forward = XMVector3Normalize(toEnemy);
    _float lockYaw = atan2f(XMVectorGetX(forward), XMVectorGetZ(forward));

    // m_fLockOnPitchThreshold 기준으로 가중치 계산
    _float fFarDist = 5.0f; //최대 거리 세팅 
    _float fWeight = 1.0f - clamp((fDist - m_fLockOnPitchThreshold) / (fFarDist - m_fLockOnPitchThreshold), 0.f, 1.f);

    // 거리에 따라 동적 Pitch로 direction 계산
    _float fDynamicPitch = fLerp(m_fLockOnPitchFar, m_fLockOnPitchNear, fWeight);

    // Yaw/Pitch 보간 전환
    if (m_bLockOnBlending)
    {
        _float fRatio = 1.0f - expf(-m_fLockOnBlendSpeed * fTimeDelta);

        // Yaw는 최단 경로 보간 (+-PI 넘어가는 경우 처리)
        _float fYawDiff = lockYaw - m_fYaw;
        if (fYawDiff > XM_PI)  fYawDiff -= XM_2PI;
        if (fYawDiff < -XM_PI) fYawDiff += XM_2PI;

        m_fYaw += fYawDiff * fRatio;
        m_fPitch = fLerp(m_fPitch, fDynamicPitch, fRatio);

        // 보간 완료 판정
        if (fabsf(fYawDiff) < m_fLockOnBlendThreshold &&
            fabsf(m_fPitch - fDynamicPitch) < m_fLockOnBlendThreshold)
        {
            m_bLockOnBlending = false;
        }
    }
    else
    {
        m_fYaw = lockYaw;
        m_fPitch = fDynamicPitch;
    }

    // 보간된 Yaw/Pitch로 direction 계산
    _matrix matRotation = XMMatrixRotationRollPitchYaw(m_fPitch, m_fYaw, 0.f);
    _vector direction = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, -1.f, 0.f), matRotation);

    // 벽 충돌 거리 계산
    _float fAllowedDistance = Compute_WallDistance(pivot, direction, m_fLockOnDistance);
    Update_Distance(fTimeDelta, fAllowedDistance);

    // 락온 전용 보간 피봇
    if (!m_bLookAtLockInitialized)
    {
        XMStoreFloat4(&m_vSmoothLookAtLock, pivot);
        m_bLookAtLockInitialized = true;
    }
    else
    {
        //  마우스 속도로 빠르게 추종
        _float fRatio = 1.0f - expf(-m_fLookAtMouseSpeed * fTimeDelta);
        _vector vSmooth = XMVectorLerp(XMLoadFloat4(&m_vSmoothLookAtLock), pivot, fRatio);
        XMStoreFloat4(&m_vSmoothLookAtLock, vSmooth);
    }

    _vector vSmoothLock = XMLoadFloat4(&m_vSmoothLookAtLock);

    // 좌우 오프셋 제거
    _vector vPosition = XMVectorMultiplyAdd(direction, XMVectorReplicate(fAllowedDistance), vSmoothLock);
    XMStoreFloat4(&m_vLogicalPosition, vPosition);
}
/******************************************************* 락온 함수 *******************************************************/



//////////////////////////////////////////////////////// Raycast 벽 거리 계산 함수 ////////////////////////////////////////////////////////
_float Camera_Player::Compute_WallDistance(_fvector _pivot, _fvector _direction, _float _maxDistance)
{
    PxVec3 _pxOrigin = ToPxVec3(_pivot);
    PxVec3 _pxDir = ToPxVec3(_direction);
    PxRaycastBuffer _hit = {};

    PxQueryFilterData _filterData;
    _filterData.flags = PxQueryFlag::eSTATIC;

    if (m_pGameInstance->Get_Scene()->raycast(
        _pxOrigin, _pxDir, _maxDistance + m_fCameraWallOffset, _hit,
        PxHitFlag::eDEFAULT, _filterData))
    {
        // 부모의 m_fCameraMinDistance 사용
        return max(_hit.block.distance - m_fCameraWallOffset, m_fCameraMinDistance);
    }

    return _maxDistance;
}
/******************************************************* Raycast 벽 거리 계산 함수 *******************************************************/



//////////////////////////////////////////////////////// 거리 보간 함수 ////////////////////////////////////////////////////////
void Camera_Player::Update_Distance(const _float fTimeDelta, _float _allowedDistance)
{
    //if (m_fCurrentDistance > _allowedDistance)
    //    m_fCurrentDistance = fLerp(m_fCurrentDistance, _allowedDistance, fTimeDelta * m_fZoomInSpeed);
    //else
    //    m_fCurrentDistance = fLerp(m_fCurrentDistance, _allowedDistance, fTimeDelta * m_fZoomOutSpeed);

    if (m_fCurrentDistance > _allowedDistance)
    {
        // 줌인은 빠르게 + EaseOutQuad로 끝부분 부드럽게
        _float t = clamp(fTimeDelta * m_fZoomInSpeed, 0.f, 1.f);
        m_fCurrentDistance = fLerp(m_fCurrentDistance, _allowedDistance, EaseOutQuad(t));
    }
    else
    {
        // 줌아웃은 느리게 + SmoothStep으로 자연스럽게
        _float t = clamp(fTimeDelta * m_fZoomOutSpeed, 0.f, 1.f);
        m_fCurrentDistance = fLerp(m_fCurrentDistance, _allowedDistance, SmoothStep(t));
    }
}
/******************************************************* 거리 보간 함수 *******************************************************/



//////////////////////////////////////////////////////// 위치 적용 함수 ////////////////////////////////////////////////////////
void Camera_Player::Apply_Position(_fvector _pivot, _fvector _direction)
{
    // 거리 보간은 Update_Distance에서 이미 처리하므로 여기선 직접 적용
    _vector vCamDest = _pivot + _direction * m_fCurrentDistance;
    m_pTransformCom->Set_State(DIRECTION::POSITION, vCamDest);
}
/******************************************************* 위치 적용 함수 *******************************************************/



//////////////////////////////////////////////////////// 카메라 흔들림 함수 ////////////////////////////////////////////////////////
void Client::Camera_Player::Start_Shake(const CameraShake& _shake, _fvector _kickDir)
{
    // CameraShake 통째로 복사
    CAST(CameraShake&)(m_tShake) = _shake;

    m_tShake.bActive = true;
    m_tShake.fRemaining = _shake.fDuration;
    m_tShake.fElapsed = 0.f;

    // 랜덤 위상
    m_tShake.fPhaseX = (_float)(rand() % 1000) * 0.001f * XM_2PI;
    m_tShake.fPhaseY = (_float)(rand() % 1000) * 0.001f * XM_2PI;
    m_tShake.fPhaseZ = (_float)(rand() % 1000) * 0.001f * XM_2PI;
    m_tShake.fPhasePitch = (_float)(rand() % 1000) * 0.001f * XM_2PI;
    m_tShake.fPhaseYaw = (_float)(rand() % 1000) * 0.001f * XM_2PI;
    m_tShake.fPhaseFov = (_float)(rand() % 1000) * 0.001f * XM_2PI;

    // 킥 방향
    if (XMVectorGetX(XMVector3LengthSq(_kickDir)) > 0.001f)
        XMStoreFloat4(&m_tShake.vKickDirection, XMVector3Normalize(_kickDir));
    else
        XMStoreFloat4(&m_tShake.vKickDirection, XMVectorZero());
}

void Client::Camera_Player::Update_Shake(const _float fTimeDelta)
{
    if (!m_tShake.bActive)
        return;

    m_tShake.fRemaining -= fTimeDelta;
    m_tShake.fElapsed += fTimeDelta;

    if (m_tShake.fRemaining <= 0.f)
    {
        m_tShake.bActive = false;
        m_pTransformCom->Set_State(DIRECTION::POSITION, m_vLogicalPosition);
        m_tShake.ePriority = SHAKE_PRIORITY::NONE;
        // FOV 복구
        m_fFov = m_fBaseFov;
        return;
    }

    _float fTime = m_tShake.fElapsed;

    // 감쇠 계산 (안 함)
    //_float fRatio = m_tShake.fRemaining / m_tShake.fDuration;
    _float fDecay = 1.f;

    // BlendOut 구간에서 추가 페이드아웃
    if (m_tShake.fBlendOutTime > 0.f && m_tShake.fRemaining < m_tShake.fBlendOutTime)
        fDecay = m_tShake.fRemaining / m_tShake.fBlendOutTime;

    // 축별 독립 Amplitude 적용
    _float fOffsetX = sinf(XM_2PI * m_tShake.fFreqX * fTime + m_tShake.fPhaseX);
    _float fOffsetY = sinf(XM_2PI * m_tShake.fFreqY * fTime + m_tShake.fPhaseY);
    _float fOffsetZ = sinf(XM_2PI * m_tShake.fFreqZ * fTime + m_tShake.fPhaseZ);

    fOffsetX *= m_tShake.fAmpX * fDecay;
    fOffsetY *= m_tShake.fAmpY * fDecay;
    fOffsetZ *= m_tShake.fAmpZ * fDecay;

    // 임팩트 킥 (초기 방향 충격)
    _float fKick = m_tShake.fKickStrength * expf(-m_tShake.fKickDecay * fTime);
    _vector vKickOffset = XMLoadFloat4(&m_tShake.vKickDirection) * fKick;

    // 카메라 로컬 축 기준으로 오프셋 적용
    _vector vLogicalPos = XMLoadFloat4(&m_vLogicalPosition);
    _vector vRight = m_pTransformCom->Get_State(DIRECTION::RIGHT);
    _vector vUp = m_pTransformCom->Get_State(DIRECTION::UP);
    _vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);

    _vector vShakePos = XMVectorMultiplyAdd(vRight, XMVectorReplicate(fOffsetX), vLogicalPos);
    vShakePos = XMVectorMultiplyAdd(vUp, XMVectorReplicate(fOffsetY), vShakePos);
    vShakePos = XMVectorMultiplyAdd(vLook, XMVectorReplicate(fOffsetZ), vShakePos);
    vShakePos = XMVectorAdd(vShakePos, vKickOffset);

    m_pTransformCom->Set_State(DIRECTION::POSITION, vShakePos);

    // 축별 독립 회전 진동 (Pitch + Yaw)
    _float fShakePitch = sinf(XM_2PI * m_tShake.fFreqPitch * fTime + m_tShake.fPhasePitch)
        * m_tShake.fAmpPitch * fDecay;
    _float fShakeYaw = sinf(XM_2PI * m_tShake.fFreqYaw * fTime + m_tShake.fPhaseYaw)
        * m_tShake.fAmpYaw * fDecay;

    _matrix matShakeRot = XMMatrixRotationAxis(vUp, fShakeYaw)
        * XMMatrixRotationAxis(vRight, fShakePitch);

    _vector vNewRight = XMVector3Normalize(XMVector3TransformNormal(vRight, matShakeRot));
    _vector vNewUp = XMVector3Normalize(XMVector3TransformNormal(vUp, matShakeRot));

    m_pTransformCom->Set_State(DIRECTION::RIGHT, vNewRight);
    m_pTransformCom->Set_State(DIRECTION::UP, vNewUp);

    // FOV 진동
    if (m_tShake.fAmpFov > 0.f)
    {
        _float fFovOffset = sinf(XM_2PI * m_tShake.fFreqFov * fTime + m_tShake.fPhaseFov)
            * m_tShake.fAmpFov * fDecay;
        m_fFov = m_fBaseFov + XMConvertToRadians(fFovOffset);
    }
}
/******************************************************* 카메라 흔들림 함수 *******************************************************/



//////////////////////////////////////////////////////// 시네마틱 카메라 함수 ////////////////////////////////////////////////////////
void Client::Camera_Player::Start_Cinematic(const vector<CINEMATIC_KEYFRAME>& _keyFrames, _bool _autoReturn)
{
    if (_keyFrames.empty())
    {
        return;
    }

    // 시네마틱 이 전의 값들 저장
    m_fSnapshotYaw      = m_fYaw;
    m_fSnapshotPitch    = m_fPitch;
    m_fSnapshotDistance = m_fCurrentDistance;
    m_vSnapshotLookAt   = m_vLogicalPosition;

    /* 시네마틱 키프레임 저장 및 초기화 */
    m_vecKeyFrame = _keyFrames;
    m_iCurrentKeyIndex = 0;
    m_fCinematicElapsed = 0.f;
    m_bCinematicAutoReturn = _autoReturn;
    m_bAutoReturning = false;
    
    // 첫 프레임은 이전 프레임값 저장 X
    if (m_vecKeyFrame[0].eMode == KEYFRAME_MODE::RELATIVEMODE)
    {
        m_bCinematicUseSnap = true;

        // 추가됨 : 스냅샷 초기값 세팅 (현재 카메라 상태)
        XMStoreFloat3(&m_fCinematicSnapPos, XMLoadFloat4(&m_vLogicalPosition));
        m_fCinematicSnapYaw = m_fYaw;
        m_fCinematicSnapPitch = m_fPitch;
        m_fCinematicSnapFov = m_fFov;
    }
    else
    {
        m_bCinematicUseSnap = false;
    }

    m_eCameraState = CAMERA_STATE::CINEMATIC;

    // 첫 구간 이벤트 플래그 리셋
    if (m_vecKeyFrame.size() > 1)
    {
        for (auto& tEvent : m_vecKeyFrame[1].vecEvents)
            tEvent.bFired = false;
    }
}

void Camera_Player::Resolve_Keyframe(const CINEMATIC_KEYFRAME& _key, _fvector _playerPivot, _fvector _yakumoPivot, _float3& _outPosition, _float& _outYaw, _float& _outPitch)
{
    if (_key.eMode == KEYFRAME_MODE::ABSOLUTEMODE)
    {
        _outPosition = _key.vAbsPosition;

        _vector lookTarget = {};
        switch (_key.eLookAt)
        {
        case CINEMATIC_LOOKAT::PLAYER:
            lookTarget = _playerPivot;
            break;
        case CINEMATIC_LOOKAT::YAKUMO:
            lookTarget = _yakumoPivot;
            break;
        case CINEMATIC_LOOKAT::CUSTOM_TARGET:
        default:
            lookTarget = XMLoadFloat3(&_key.vlookAtTarget);
            break;
        }

        _vector toTarget = XMVectorSubtract(lookTarget, XMLoadFloat3(&_key.vAbsPosition));

        _vector toTargetXZ = XMVectorSetY(toTarget, 0.f);
        _float horizontalLen = XMVectorGetX(XMVector3Length(toTargetXZ));

        if (horizontalLen > 0.001f)
        {
            _vector dir = XMVector3Normalize(toTargetXZ);
            _outYaw = atan2f(XMVectorGetX(dir), XMVectorGetZ(dir));
        }
        else
        {
            _outYaw = m_fSnapshotYaw;
        }

        _float totalDist = XMVectorGetX(XMVector3Length(toTarget));
        if (totalDist > 0.001f)
            _outPitch = -asinf(XMVectorGetY(toTarget) / totalDist);
        else
            _outPitch = 0.f;
    }
    else if (_key.eMode == KEYFRAME_MODE::PLAYER_OFFSET || _key.eMode == KEYFRAME_MODE::YAKUMO_OFFSET) // 수정됨 : YAKUMO_OFFSET 통합
    {
        // 수정됨 : 모드에 따라 기준 대상 결정
        _vector basePivot = {};
        _float baseYaw = {};

        if (_key.eMode == KEYFRAME_MODE::PLAYER_OFFSET)
        {
            Player* player = dynamic_cast<Player*>(m_pTarget);
            _vector playerLook = XMVector3Normalize(
                XMVectorSetY(player->Get_Look(), 0.f));
            baseYaw = atan2f(XMVectorGetX(playerLook), XMVectorGetZ(playerLook));
            basePivot = _playerPivot;
        }
        else // YAKUMO_OFFSET
        {
            // 추가됨 : Yakumo의 Look 방향 기준
            if (m_pYakumo)
            {
                _vector yakumoLook = XMVector3Normalize(
                    XMVectorSetY(CAST(GameObject*)(m_pYakumo)->Get_Look(), 0.f));
                baseYaw = atan2f(XMVectorGetX(yakumoLook), XMVectorGetZ(yakumoLook));
            }
            else
            {
                baseYaw = m_fSnapshotYaw;
            }
            basePivot = _yakumoPivot;
        }

        _float cosY = cosf(baseYaw);
        _float sinY = sinf(baseYaw);

        // 위치: 기준 대상 로컬 오프셋을 월드로 변환
        _outPosition.x = XMVectorGetX(basePivot)
            + _key.vPosOffset.x * cosY + _key.vPosOffset.z * sinY;
        _outPosition.y = XMVectorGetY(basePivot)
            + _key.vPosOffset.y;
        _outPosition.z = XMVectorGetZ(basePivot)
            - _key.vPosOffset.x * sinY + _key.vPosOffset.z * cosY;

        // LookAt 대상 결정
        _vector lookTarget = {};
        switch (_key.eLookAt)
        {
        case CINEMATIC_LOOKAT::PLAYER:
            lookTarget = _playerPivot;
            break;
        case CINEMATIC_LOOKAT::YAKUMO:
            lookTarget = _yakumoPivot;
            break;
        case CINEMATIC_LOOKAT::CUSTOM_TARGET:
        default:
        {
            _matrix matBaseRot = XMMatrixRotationY(baseYaw);
            _vector localLookAt = XMLoadFloat3(&_key.vlookAtTarget);
            lookTarget = XMVectorAdd(
                basePivot,
                XMVector3TransformNormal(localLookAt, matBaseRot));
            break;
        }
        }

        _vector toTarget = XMVectorSubtract(lookTarget, XMLoadFloat3(&_outPosition));

        _vector toTargetXZ = XMVectorSetY(toTarget, 0.f);
        _float horizontalLen = XMVectorGetX(XMVector3Length(toTargetXZ));

        if (horizontalLen > 0.001f)
        {
            _vector dir = XMVector3Normalize(toTargetXZ);
            _outYaw = atan2f(XMVectorGetX(dir), XMVectorGetZ(dir));
        }
        else
        {
            _outYaw = baseYaw;
        }

        _float totalDist = XMVectorGetX(XMVector3Length(toTarget));
        if (totalDist > 0.001f)
            _outPitch = -asinf(XMVectorGetY(toTarget) / totalDist);
        else
            _outPitch = 0.f;
    }
    else // RELATIVE
    {
        _float cosY = cosf(m_fSnapshotYaw);
        _float sinY = sinf(m_fSnapshotYaw);

        _outPosition.x = XMVectorGetX(_playerPivot) + _key.vPosOffset.x * cosY + _key.vPosOffset.z * sinY;
        _outPosition.y = XMVectorGetY(_playerPivot) + _key.vPosOffset.y;
        _outPosition.z = XMVectorGetZ(_playerPivot) - _key.vPosOffset.x * sinY + _key.vPosOffset.z * cosY;

        _outYaw = m_fSnapshotYaw + _key.fYawOffset;
        _outPitch = m_fSnapshotPitch + _key.fPitchOffset;
    }
}

void Camera_Player::Compute_Cinematic(const _float fTimeDelta)
{
    if (m_bAutoReturning)
    {
        Compute_CinematicReturn(fTimeDelta);
        return;
    }

    if (m_vecKeyFrame.empty())
    {
        End_Cinematic();
        return;
    }

    // ==================== 1단계: 시간 진행 ====================
    m_fCinematicElapsed += fTimeDelta;

    if (m_iCurrentKeyIndex + 1 >= m_vecKeyFrame.size())
    {
        End_Cinematic();
        return;
    }

    _float segmentDuration = m_vecKeyFrame[m_iCurrentKeyIndex + 1].fDuration;
    if (segmentDuration <= 0.f)
        segmentDuration = 0.001f;

    _float rawT = clamp(m_fCinematicElapsed / segmentDuration, 0.f, 1.f);
    _float t = ApplyEase(rawT, m_vecKeyFrame[m_iCurrentKeyIndex + 1].eEaseType);


    // ==================== 2단계: from/to 좌표 구하기 ====================
    Player* player = CAST(Player*)(m_pTarget);
    _vector playerPivot = XMVectorAdd(
        player->Get_RenderPos(),
        XMVectorSet(0.f, m_fPivotHeight, 0.f, 0.f));

    // 추가됨 : Yakumo 피봇 계산
    _vector yakumoPivot = playerPivot; // 수정됨 : fallback은 플레이어 위치
    if (m_pYakumo)
    {
        yakumoPivot = XMVectorAdd(
            CAST(GameObject*)(m_pYakumo)->Get_Position(),
            XMVectorSet(0.f, m_fPivotHeight, 0.f, 0.f));
    }

    // from은 스냅샷 우선, 없으면 Resolve
    _float3 fromPos = {};
    _float  fromYaw = {};
    _float  fromPitch = {};
    _float  fromFov = {};

    if (m_bCinematicUseSnap)
    {
        fromPos = m_fCinematicSnapPos;
        fromYaw = m_fCinematicSnapYaw;
        fromPitch = m_fCinematicSnapPitch;
        fromFov = m_fCinematicSnapFov;
    }
    else
    {
        Resolve_Keyframe(m_vecKeyFrame[m_iCurrentKeyIndex],
            playerPivot, yakumoPivot, fromPos, fromYaw, fromPitch); // 수정됨 : yakumoPivot 추가
        fromFov = (m_vecKeyFrame[m_iCurrentKeyIndex].fFov > 0.f)
            ? m_vecKeyFrame[m_iCurrentKeyIndex].fFov : m_fBaseFov;
    }

    _float3 toPos = {};
    _float  toYaw = {};
    _float  toPitch = {};
    Resolve_Keyframe(m_vecKeyFrame[m_iCurrentKeyIndex + 1],
        playerPivot, yakumoPivot, toPos, toYaw, toPitch); // 수정됨 : yakumoPivot 추가



    // ==================== 3단계: 보간 ====================

    // 위치: Lerp (유클리드 공간이니까 직선이 맞음)
    _float3 blendedPos{};
    blendedPos.x = fromPos.x + (toPos.x - fromPos.x) * t;
    blendedPos.y = fromPos.y + (toPos.y - fromPos.y) * t;
    blendedPos.z = fromPos.z + (toPos.z - fromPos.z) * t;
    XMStoreFloat4(&m_vLogicalPosition,
        XMVectorSet(blendedPos.x, blendedPos.y, blendedPos.z, 1.f));

    // 회전 : Slerp
    // Yaw/Pitch -> 쿼터니언 변환
    _vector fromQuat = XMQuaternionRotationRollPitchYaw(fromPitch, fromYaw, 0.f);
    _vector toQuat = XMQuaternionRotationRollPitchYaw(toPitch, toYaw, 0.f);

    // 음수 dot이면 뒤집어서 짧은 경로 보장
    _float dot = XMVectorGetX(XMQuaternionDot(fromQuat, toQuat));
    if (dot < 0.f)
        toQuat = XMVectorNegate(toQuat);

    // 구면 보간
    _vector blendedQuat = XMQuaternionSlerp(fromQuat, toQuat, t);

    // 쿼터니언 -> Yaw/Pitch 역추출
    _matrix rotMatrix = XMMatrixRotationQuaternion(blendedQuat);
    _float4x4 mat = {};
    XMStoreFloat4x4(&mat, rotMatrix);

    m_fPitch = asinf(-mat._32);
    m_fYaw = atan2f(mat._31, mat._33);
    m_fPitch = clamp(m_fPitch, m_fMinPitch, m_fMaxPitch);

    // FOV: 0이면 현재 유지
    _float toFov = (m_vecKeyFrame[m_iCurrentKeyIndex + 1].fFov > 0.f)
        ? m_vecKeyFrame[m_iCurrentKeyIndex + 1].fFov : m_fBaseFov;
    m_fFov = fromFov + (toFov - fromFov) * t;


    // ==================== 3.5단계: 시네마틱 이벤트 발행 ====================
    auto& vecEvents = m_vecKeyFrame[m_iCurrentKeyIndex + 1].vecEvents;
    for (auto& tEvent : vecEvents)
    {
        if (tEvent.bFired)
            continue;

        if (m_fCinematicElapsed >= tEvent.fTriggerTime)
        {
            Fire_CinematicEvent(tEvent);
            tEvent.bFired = true;
        }
    }


    // ==================== 4단계: 키프레임 넘어가기 ====================
    if (m_fCinematicElapsed >= segmentDuration)
    {
        // 전환 직전의 현재 화면 값을 스냅샷으로 저장
        m_fCinematicSnapPos = blendedPos;
        m_fCinematicSnapYaw = m_fYaw;
        m_fCinematicSnapPitch = m_fPitch;
        m_fCinematicSnapFov = m_fFov;
        m_bCinematicUseSnap = true;

        m_fCinematicElapsed -= segmentDuration;
        m_iCurrentKeyIndex++;

        if (m_iCurrentKeyIndex + 1 < (_uint)m_vecKeyFrame.size())
        {
            for (auto& tEvent : m_vecKeyFrame[m_iCurrentKeyIndex + 1].vecEvents)
                tEvent.bFired = false;
        }
    }
}

void Camera_Player::Compute_CinematicReturn(const _float fTimeDelta)
{
    m_fReturnElapsed += fTimeDelta;
    _float rawT = clamp(m_fReturnElapsed / m_fReturnDuration, 0.f, 1.f);
    _float t = SmoothStep(rawT);

    Player* player = CAST(Player*)(m_pTarget);
    _vector playerRenderPos = player->Get_RenderPos();
    _vector playerPivot = XMVectorAdd(
        playerRenderPos,
        XMVectorSet(0.f, m_fPivotHeight, 0.f, 0.f));

    // 복귀 목표를 매 프레임 실시간으로 계산
    _float3 targetPosF3 = {};
    _float  targetYaw = {};
    _float  targetPitch = {};

    if (m_ePrevState == CAMERA_STATE::LOCK_ON && m_pLockOnTarget)
    {
        // 수정됨 : LockOn 상태였으면 있었을 위치 계산
        _vector enemyPos = m_pLockOnTarget->Get_Position();
        _vector toEnemy = XMVectorSetY(
            XMVectorSubtract(enemyPos, playerRenderPos), 0.f);
        _float fDist = XMVectorGetX(XMVector3Length(toEnemy));

        _vector forward = XMVector3Normalize(toEnemy);
        targetYaw = atan2f(XMVectorGetX(forward), XMVectorGetZ(forward));

        // 수정됨 : 거리 기반 동적 Pitch (Compute_LockOn과 동일 로직)
        _float fFarDist = 5.0f;
        _float fWeight = 1.0f - clamp(
            (fDist - m_fLockOnPitchThreshold) / (fFarDist - m_fLockOnPitchThreshold), 0.f, 1.f);
        targetPitch = fLerp(m_fLockOnPitchFar, m_fLockOnPitchNear, fWeight);

        // 수정됨 : 거리 기반 Y오프셋 (Compute_LockOn과 동일 로직)
        _float yOffset = m_fLockOnYOffsetMax;
        if (fDist > m_fLockOnYOffsetStartDist)
        {
            _float tY = clamp(
                (fDist - m_fLockOnYOffsetStartDist) / (m_fLockOnYOffsetEndDist - m_fLockOnYOffsetStartDist),
                0.f, 1.f);
            yOffset = m_fLockOnYOffsetMax * (1.f - tY);
        }

        _vector pivot = XMVectorAdd(playerRenderPos,
            XMVectorSet(0.f, m_fPivotHeight + m_fRockOnPivotHeight + yOffset, 0.f, 0.f));

        _matrix matRot = XMMatrixRotationRollPitchYaw(targetPitch, targetYaw, 0.f);
        _vector dir = XMVector3TransformNormal(
            XMVectorSet(0.f, 0.f, -1.f, 0.f), matRot);
        _vector targetPos = XMVectorMultiplyAdd(
            dir, XMVectorReplicate(m_fLockOnDistance), pivot);
        XMStoreFloat3(&targetPosF3, targetPos);
    }
    else
    {
        // 수정됨 : FreeFollow 상태였으면 있었을 위치 계산
        // 시네마틱 진입 전 Yaw/Pitch 기준으로 복귀
        targetYaw = m_fSnapshotYaw;
        targetPitch = m_fSnapshotPitch;

        _matrix matRot = XMMatrixRotationRollPitchYaw(targetPitch, targetYaw, 0.f);
        _vector dir = XMVector3TransformNormal(
            XMVectorSet(0.f, 0.f, -1.f, 0.f), matRot);

        // 수정됨 : 벽 충돌 고려
        _float allowedDist = Compute_WallDistance(playerPivot, dir, m_fDistance);

        _vector targetPos = XMVectorMultiplyAdd(
            dir, XMVectorReplicate(allowedDist), playerPivot);
        XMStoreFloat3(&targetPosF3, targetPos);
    }

    // 위치: Lerp
    _float3 blendedPos = {};
    blendedPos.x = m_returnStartPos.x + (targetPosF3.x - m_returnStartPos.x) * t;
    blendedPos.y = m_returnStartPos.y + (targetPosF3.y - m_returnStartPos.y) * t;
    blendedPos.z = m_returnStartPos.z + (targetPosF3.z - m_returnStartPos.z) * t;
    XMStoreFloat4(&m_vLogicalPosition,
        XMVectorSet(blendedPos.x, blendedPos.y, blendedPos.z, 1.f));

    // 회전: Slerp
    XMVECTOR fromQuat = XMQuaternionRotationRollPitchYaw(
        m_returnStartPitch, m_returnStartYaw, 0.f);
    XMVECTOR toQuat = XMQuaternionRotationRollPitchYaw(
        targetPitch, targetYaw, 0.f);

    _float dot = XMVectorGetX(XMQuaternionDot(fromQuat, toQuat));
    if (dot < 0.f)
        toQuat = XMVectorNegate(toQuat);

    XMVECTOR blendedQuat = XMQuaternionSlerp(fromQuat, toQuat, t);

    XMMATRIX rotMat = XMMatrixRotationQuaternion(blendedQuat);
    XMFLOAT4X4 mat = {};
    XMStoreFloat4x4(&mat, rotMat);

    m_fPitch = asinf(-mat._32);
    m_fYaw = atan2f(mat._31, mat._33);
    m_fPitch = clamp(m_fPitch, m_fMinPitch, m_fMaxPitch);

    // FOV 복귀
    m_fFov = m_returnStartFov + (m_fBaseFov - m_returnStartFov) * t;

    // 완료 판정
    if (rawT >= 1.f)
    {
        m_bAutoReturning = false;
        m_eCameraState = m_ePrevState;
        m_fYaw = targetYaw;
        m_fPitch = targetPitch;
        m_fFov = m_fBaseFov;
        m_fCurrentDistance = (m_ePrevState == CAMERA_STATE::LOCK_ON)
            ? m_fLockOnDistance : m_fDistance;

        // 추가됨 : 복귀 완료 후 LookAt 피봇 스냅 (전환 시 튀지 않게)
        if (m_ePrevState == CAMERA_STATE::LOCK_ON)
        {
            XMStoreFloat4(&m_vSmoothLookAtLock, playerPivot);
            m_bLookAtLockInitialized = true;
        }
        else
        {
            XMStoreFloat4(&m_vSmoothLookAtFree, playerPivot);
            m_bLookAtFreeInitialized = true;
        }
    }
}

void Client::Camera_Player::Fire_CinematicEvent(const CINEMATIC_EVENT& _event)
{
    switch (_event.eType)
    {
    case CINEMATIC_EVENT_TYPE::CAMERA_SHAKE:
    {
        const auto& shake = get<CameraShake>(_event.tPayload);
        Start_Shake(shake, XMLoadFloat4(&shake.vKickDirection));
        break;
    }
    case CINEMATIC_EVENT_TYPE::CHARACTER_CONTROL:
    {
        const auto& control = get<CINEMATIC_CHARACTER_CONTROLL>(_event.tPayload);
        m_pGameInstance->Publish(control);
        break;
    }
    default:
        break;
    }
}

void Camera_Player::End_Cinematic()
{
    m_vecKeyFrame.clear();
    m_iCurrentKeyIndex = 0;
    m_fCinematicElapsed = 0.f;
    m_bCinematicUseSnap = false;

    if (m_bCinematicAutoReturn)
    {
        XMStoreFloat3(&m_returnStartPos, XMLoadFloat4(&m_vLogicalPosition));
        m_returnStartYaw = m_fYaw;
        m_returnStartPitch = m_fPitch;
        m_returnStartFov = m_fFov;
        m_fReturnElapsed = 0.f;
        m_bAutoReturning = true;
    }
    else
    {
        m_eCameraState = m_ePrevState;
        m_fYaw = m_fSnapshotYaw;
        m_fPitch = m_fSnapshotPitch;
    }
}
/******************************************************* 시네마틱 카메라 함수 *******************************************************/




//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Camera_Player* Camera_Player::Create(ID3D11Device* _device, ID3D11DeviceContext* _context, LEVEL _level)
{
    Camera_Player* pInstance = new Camera_Player(_device, _context);
    MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Camera_Player 원본 생성 실패", L"경고!!!", nullptr);
    return pInstance;
}

GameObject* Camera_Player::Clone(void* _arg)
{
    Camera_Player* pInstance = new Camera_Player(*this);
    MSG_FAIL(pInstance->Initialize(_arg), L"Camera_Player 복사 실패", L"경고!!!", nullptr);
    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Camera_Player::Free()
{
    __super::Free();

    Safe_Release(m_pLockOnTarget);
    Safe_Release(m_pTarget);
}
/******************************************************* 객체 반환 함수 *******************************************************/
