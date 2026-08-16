#pragma once

#include "GameObject.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

NS_BEGIN(Engine)

class ENGINE_DLL Camera abstract : public GameObject
{
public:
    typedef struct tagCameraDesc : public GameObject::GAMEOBJECT_DESC
    {
        _float3 vEye = {};
        _float3 vAt = {};

        _float fFov = {};
        _float fNear = {};
        _float fAspect = {};
        _float fFar = {};

        _float fSmoothness = {};
    }CAMERA_DESC;

    typedef struct tagCameraKeyFrame
    {
        _float fTrackPosition = 0.f;   // 시간
        _float3 vPosition = {};         // 위치
        _float4 vRotation = {};         // 회전 (Quaternion)
    }CAMERA_KEYFRAME;

protected:
    explicit Camera();
    explicit Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Camera(const Camera& original);
    virtual ~Camera();

public:
    HRESULT Initialize_Prototype();
    HRESULT Initialize(void* arg);
    _int	Update_Priority(const _float fTimeDelta);
    _int	Update(const _float fTimeDelta);
    virtual void Update_Camera(const _float fTimeDelta) = 0;
    _int	Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta);

    HRESULT Bind_PipeLine();

public:
    __forceinline void IsChanged(_bool _changed) { m_bIsChanged = _changed; }
    __forceinline void Change_Fov(const _float _fov) { m_fFov = _fov; }
    __forceinline void Change_Aspect(const _float _aspect) { m_fAspect = _aspect; }
    __forceinline void Camera_Lock(_bool _lockMoving) { m_bLock = _lockMoving; }

#pragma region 카메라 애니메이션
    // 카메라 애니메이션 관련 함수
    HRESULT Load_CameraAnimation(const _tchar* _filePath, const _char* _cameraNodeName = nullptr);
    void    Play_CameraAnimation(_bool _isLoop = false);
    void    Stop_CameraAnimation();
    void    Reset_CameraAnimation();
    _bool   Is_AnimationFinished() const { return m_bAnimFinished; }
    _bool   Is_AnimationPlaying() const { return m_bIsPlayingAnim; }
    _float  Get_AnimationProgress() const { return m_fAnimDuration > 0.f ? m_fCurrentTrackPosition / m_fAnimDuration : 0.f; }
    void    Update_CameraAnimation(const _float fTimeDelta);
    virtual _vector Apply_CameraSweep(_fvector _targetCenter, _fvector _camDest);
    virtual _float4 Get_LookAtPosition() { return _float4(0.f, 0.f, 0.f, 1.f); }
#pragma endregion 카메라 애니메이션

public:
    //Get함수
    virtual _float      Get_Yaw() {return m_fYaw;}
protected:
    _bool m_bIsChanged = { false };
    _bool  m_bLock = { true };

    _float3 m_vEye = {};
    _float3 m_vAt = {};

    _float  m_fFov = {};
    _float  m_fNear = {};
    _float  m_fAspect = {};
    _float  m_fFar = {};

    _float m_fYaw = {};
    _float m_fPitch = {};
    _float m_fTargetYaw = {};
    _float m_fTargetPitch = {};
    _float m_fMaxPitch = XMConvertToRadians(89.f);
    _float m_fSmoothness = 17.f;   // 보간 속도 (높을수록 빠름)

    // 카메라 애니메이션 관련 멤버 변수
    vector<CAMERA_KEYFRAME>     m_vecCamKeyFrames;
    _float                      m_fAnimDuration = 0.f;
    _float                      m_fCurrentTrackPosition = 0.f;
    _float                      m_fTicksPerSecond = 0.f;
    _uint                       m_iCurrentKeyFrameIndex = 0;
    _bool                       m_bIsPlayingAnim = false;
    _bool                       m_bAnimLoop = false;
    _bool                       m_bAnimFinished = false;

    // 카메라 Sweep 관련
    _float m_fCameraSweepRadius = 0.2f;    // Sweep 구체 반지름
    _float m_fCameraSweepMargin = 0.15f;   // 벽에서 떨어질 여유 거리
    _float m_fCameraMinDistance = 0.5f;     // 캐릭터와 카메라 최소 거리


public:
    static Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual GameObject* Clone(void* arg);

public:
    void Free() override;

};

NS_END
