#pragma once

#include "Client_Define.h"
#include "Camera.h"

NS_BEGIN(Client)

class Camera_NPC final : public Camera
{
public:
    struct TRANSITION_DESC
    {
        _float4     vTargetPosition = {};       // 목표 카메라 위치
        _float4     vTargetLookAt = {};         // 목표가 바라볼 지점 (NPC 머리 등)
        _float      fDuration = 1.5f;           // 전환 시간
        _float      fArcHeight = 2.0f;          // 호 높이 (제어점 밀어내기 강도)
    };

private:
    Camera_NPC();
    Camera_NPC(ID3D11Device* _device, ID3D11DeviceContext* _context);
    Camera_NPC(const Camera_NPC& _original);
    virtual ~Camera_NPC();

public:
    HRESULT     Initialize_Prototype(LEVEL _level);
    HRESULT     Initialize(void* _arg);

public:
    virtual _int        Update_Priority(const _float fTimeDelta) override;
    virtual _int        Update(const _float fTimeDelta) override;
    virtual void        Update_Camera(const _float fTimeDelta) override;
    virtual _int        Update_Late(const _float fTimeDelta) override;
    virtual HRESULT     Render(const _float fTimeDelta) override;

public:
    void                Start_Transition(const TRANSITION_DESC& _desc);
    _bool               Is_TransitionDone() const { return m_bTransitionDone; }

private:
    // Bezier 보간
    _vector             Compute_CubicBezier(_float _t);
    void                Compute_ControlPoints();

    // Quaternion 유틸
    _vector             LookAt_Quaternion(_fvector _eye, _fvector _target, _fvector _worldUp);

private:
    // 전환 상태
    _bool               m_bTransitioning = {};
    _bool               m_bTransitionDone = {};
    _float              m_fElapsed = {};
    _float              m_fDuration = {};

    // Bezier 제어점
    _float4             m_vP0 = {};             // 시작 위치
    _float4             m_vP1 = {};             // 제어점 1 (시작 Look 방향)
    _float4             m_vP2 = {};             // 제어점 2 (목표 Look 반대 방향)
    _float4             m_vP3 = {};             // 끝 위치

    // 회전 보간
    _float4             m_vStartQuat = {};      // 시작 회전 (Quaternion)
    _float4             m_vEndQuat = {};        // 목표 회전 (Quaternion)

    // 목표 정보
    _float4             m_vTargetLookAt = {};   // 도착 후 바라볼 지점
    _float              m_fArcHeight = {};      // 제어점 밀어내기 강도

public:
    static Camera_NPC* Create(ID3D11Device* _device, ID3D11DeviceContext* _context, LEVEL _level);
    virtual GameObject* Clone(void* _arg) override;

public:
    virtual void Free() override;
};

NS_END