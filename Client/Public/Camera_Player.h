#pragma once
#include "Client_Define.h"
#include "Camera.h"

NS_BEGIN(Client)

class Monster;

class Camera_Player final : public Camera
{
public:
    struct CAMOBJ_DESC : public Camera::CAMERA_DESC
    {
        _float fSensor = {};
    };
    // 다축 임팩트 셰이크 데이터
    struct ShakeData : public CameraShake {
        _bool   bActive = {};
        _float  fRemaining = {};
        _float  fElapsed = {};

        // 랜덤 위상 (Start_Shake에서 초기화)
        _float fPhaseX = {};
        _float fPhaseY = {};
        _float fPhaseZ = {};
        _float fPhasePitch = {};
        _float fPhaseYaw = {};
        _float fPhaseFov = {};
    };

private:
    explicit Camera_Player();
    explicit Camera_Player(ID3D11Device* _device, ID3D11DeviceContext* _context);
    explicit Camera_Player(const Camera_Player& _original);
    virtual ~Camera_Player();
public:
    HRESULT Initialize_Prototype(LEVEL _level);
    HRESULT Initialize(void* _arg);
    _int    Update_Priority(const _float fTimeDelta);
    _int    Update_Parallel(const _float fTimeDelta);
    _int    Update(const _float fTimeDelta);
    void    Update_Camera(const _float fTimeDelta) override;
    _int    Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta);
    HRESULT Ready_Value(void* _arg);

public:
    void Event_Set(const CameraEvent& _event);
    /* 락온 타겟 지정 */
    void Set_LockOnTarget(GameObject* _target);
    /* 죽었으면 이걸로 해제 */
    void Clear_LockOnTarget();
    /* 이걸로 락온 */
    void Toggle_LockOn();
    /* 따라다닐 객체 */
    void Set_Target(GameObject* _target) { m_pTarget = _target; }
    /* 킥 파라미터 */
    virtual _float4 Get_LookAtPosition() override { return m_vSmoothLookAtFree; }

    void Start_Shake(const CameraShake& _shake, _fvector _kickDir);
private:
    void    Compute_FreeFollow(const _float fTimeDelta);
    void    Compute_LockOn(const _float fTimeDelta);
    // 마우스 움직임으로 카메라 각도 구하기
    void    Process_MouseInput(const _float fTimeDelta);
    _float  Compute_WallDistance(_fvector _pivot, _fvector _direction, _float _maxDistance);
    void    Update_Distance(const _float fTimeDelta, _float _allowedDistance);
    void    Update_Shake(const _float fTimeDelta);
    // 지금은 안 쓰는 함수(일단 보존)
    void    Apply_Position(_fvector _pivot, _fvector _direction);

#pragma region 시네마틱 카메라
    // 시네마틱 시작시 현재 카메라값 저장용
    void Start_Cinematic(const vector<CINEMATIC_KEYFRAME>& _keyFrames, _bool _autoReturn);
    // 시네마틱 키프레임의 카메라 위치 계산
    void Resolve_Keyframe(const CINEMATIC_KEYFRAME& _key, _fvector _playerPivot, _fvector _yakumoPivot, _float3& _outPosition, _float& _outYaw, _float& _outPitch);
    // 시네마틱 계산
    void Compute_Cinematic(const _float fTimeDelta);
    void Compute_CinematicReturn(const _float fTimeDelta);
    // 시네마틱 이벤트 발행
    void Fire_CinematicEvent(const CINEMATIC_EVENT& _event);
    // 시네마틱 끝
    void End_Cinematic();

private:
    /* 시네마틱 이 전의 값들 저장 */
    CAMERA_STATE m_ePrevState = {};
    _float m_fSnapshotYaw = {};
    _float m_fSnapshotPitch = {};
    _float m_fSnapshotDistance = {};
    _float4 m_vSnapshotLookAt = {};

    /* 시네마틱 키프레임 저장 */
    vector<CINEMATIC_KEYFRAME> m_vecKeyFrame;
    _bool m_bCinematicAutoReturn = {};          // 끝나면 자동으로 원래 자리 돌아갈지
    _bool m_bAutoReturning = {};                // 자동으로 돌아가는중인지
    _uint m_iCurrentKeyIndex = {};
    _float m_fCinematicElapsed = {};            // 시네마틱 진행된 시간
    _float m_fCinematicSnapFov = {};

    /* 이 전 키프레임 값 저장 */
    _float3 m_fCinematicSnapPos = {};
    _float  m_fCinematicSnapYaw = {};
    _float  m_fCinematicSnapPitch = {};
    _bool   m_bCinematicUseSnap = {};

    /* 복귀 보간용 */
    _float  m_fReturnDuration = {};   // 복귀 시간 (조절 가능)
    _float  m_fReturnElapsed = {};
    _float3 m_returnStartPos = {};
    _float  m_returnStartYaw = {};
    _float  m_returnStartPitch = {};
    _float  m_returnStartFov = {};
#pragma endregion 시네마틱 카메라



private:
    _float m_fBaseFov = {};
    class Yakumo* m_pYakumo = { nullptr };

    // 마우스
    _float m_fSensor = {};
    _float m_fMinPitch = {};
    _float m_fMaxPitch = {};
    // 거리
    _float m_fDistance = {};
    _float m_fLockOnDistance = {};
    _float m_fCurrentDistance = {};
    // 벽 충돌
    _float m_fCameraWallOffset = 0.3f;
    _float m_fZoomInSpeed = 10.f;
    _float m_fZoomOutSpeed = 10.f;
    // 피벗
    _float m_fPivotHeight = 1.5f;
    _float m_fRockOnPivotHeight = {};
    /* 락온 */
    CAMERA_STATE m_eCameraState = CAMERA_STATE::FREE_FOLLOW;
    Monster* m_pLockOnTarget = {};
    _float m_fLockOnLookAtHeightOffset = { 1.5f };
    _float m_fLockOnPitchNear = 0.1f;   // 가까울 때 Pitch (위에서 내려다봄)
    _float m_fLockOnPitchFar = 0.0f;   // 멀 때 Pitch (거의 수평)
    _float m_fLockOnPitchThreshold = {};   // 거리 이하부터 Pitch 변화 시작
    _float m_fLockOnYOffsetMax = {};
    _float m_fLockOnYOffsetStartDist = {};
    _float m_fLockOnYOffsetEndDist = {};
    /* 락온 전환 보간 */
    _bool m_bLockOnBlending = {};        // 락온 전환 중 여부
    _float m_fLockOnBlendSpeed = {};     // 락온 전환 보간 속도
    _float m_fLockOnBlendThreshold = {}; // 보간 완료 판정 임계값

    /* 카메라 떨림 */
    ShakeData m_tShake = {};
    _float4 m_vLogicalPosition = {};

    /* 캐릭터 쫒아가기 */
    _bool m_bMouseMoved = false;
    _float4 m_vSmoothLookAtFree = {};      // 프리팔로우용 보간 LookAt위치 피봇
    _float4 m_vSmoothLookAtLock = {};      // 락온용 보간 LookAt위치 피봇
    _bool   m_bLookAtFreeInitialized = false;   // Initialize 스냅용
    _bool   m_bLookAtLockInitialized = false;   // Initialize 스냅용
    _float  m_fLookAtFollowSpeed = {};     // LookAt 추종 속도 (낮을수록 느릿느릿)
    _float  m_fLookAtMouseSpeed = {};      // 마우스 회전시 LookAt속도

public:
    static Camera_Player* Create(ID3D11Device* _device, ID3D11DeviceContext* _context, LEVEL _level);
    GameObject* Clone(void* _arg) override;
public:
    void Free() override final;

private:
    _bool        m_bStatic = false;

};

NS_END