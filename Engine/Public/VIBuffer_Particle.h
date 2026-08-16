#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Particle abstract : public VIBuffer
{
public:
	typedef struct tagVIBuffer_Particle_Desc {
		_bool IsLoop = {false};             // 파티클이 반복되는 파티클인지
		_bool IsBillboard = {true};         // 빌보드 여부
        _bool IsVerticalFade = {};          // 가로로 줄어들게 할지
        _bool IsVerticalShrink = {};        // 세로 줄어들게 할지
        _bool IsRadialRotation = { false }; // 피봇 중심으로 바라볼지
		_uint iNumInstance = {};            // 인스턴스 개수(입자의 개수)
		_float2 vSize = {};                 // 입자의 크기
		_float2 vSpeed = {};                // 입자의 속도(x ~ y 사이 값) 
		_float2 vLifeTime = {};             // 입자의 생명주기
        _float2 vStartDelay = {};           // 입자의 딜레이 시간
		_float2 vRotation = {};             // 회전 값 (x ~ y 사이 값)
		_float3 vCenter = {};               // 입자의 중점
		_float3 vRange = {};                // 입자가 퍼질 범위
		_float3 vPivot = {};                // 입자의 피벗(파티클 방향을 잡아주는 역할)
		_float  fGravity = {};              // 입자의 중력
		_float  fAlpha = { 1.f };           // 전체 알파값
        _float  fParticleScale = { 1.f };   // 입자의 최종 크기 배율
        _int    iDirectionalScale = { 0 };  // 방향을 가져 크기를 줄일 것인가
        _float  fConvergeMaxDist = {};      // CS_CONVERGE 사용할 때 모이는 최소 거리

        // CS_DUST용 변수
        _float  fShakeStrength = {};        // 흔들림 강도

		// 스프라이트 이미지 전용 변수
		_uint iFrameCount = { 1 };
		_uint iCountX = { 1 };
		_uint iCountY = { 1 };
		_uint iTransparentIndex = {};

        // 마스크 스프라이트 전용 변수
        _uint iMaskStartFrame = {};
        _uint iMaskFrameCount = { 1 };
        _uint iMaskCountX = { 1 };
        _uint iMaskCountY = { 1 };
        _float2 vMaskUVScroll = {};
	}PARTICLE_DESC;

protected:
	explicit VIBuffer_Particle();
	explicit VIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit VIBuffer_Particle(const VIBuffer_Particle& original);
	virtual ~VIBuffer_Particle();

public:
	ID3D11Buffer*   Get_InstanceBuffer() { return m_pVBInstance; }
    ID3D11Buffer*   Get_ComputeBuffer() { return m_pParticleBuffer; }
    PARTICLE_DESC   Get_ParticleDesc();
    _uint           Get_NumInstance() { return m_iNumInstance; }
    _uint           Get_IsLoop() { return m_bIsLoop; }
    
    void            Set_WorldMatrix(const _float4x4& matWorldOffset) { m_matPrevWorldOffset = m_matWorldOffset; m_matWorldOffset = matWorldOffset; }
    void            Set_FollowParent(_bool bFollow) { m_bFollowParent = (_uint)bFollow; }

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;
    HRESULT Bind_Resource() override;
    HRESULT Render(const _float fTimeDelta) override;

public:
    void                Update_Particles(const PARTICLE_DESC& desc);
    void                Reset_Particles();
    vector<_float4x4>   ReadGPU_DeadParticles();
    _float3             Convert_LocalToWorld(const _float3& vLocal);

public:
    virtual HRESULT Bind_EffectResources(class Shader* pShader);
    virtual HRESULT Bind_ComputeShaderResources(class ComputeShader* pCShader, const _float& fTimeDelta);

protected:
    virtual HRESULT Recreate_GPUBuffers() { return S_OK; }

protected:
    ID3D11Buffer*               m_pVBInstance = { nullptr };
    D3D11_BUFFER_DESC           m_InstanceBufferDesc = {};

    ID3D11Buffer*               m_pParticleBuffer = { nullptr };
    ID3D11UnorderedAccessView*  m_pParticleUAV = { nullptr };
    
    ID3D11Buffer*               m_pSRVBuffer = { nullptr };
    ID3D11ShaderResourceView*   m_pParticleSRV = { nullptr };

    ID3D11Buffer*               m_pDeadParticleBuffer = { nullptr };
    ID3D11UnorderedAccessView*  m_pDeadParticleUAV = { nullptr };
    
    ID3D11Buffer*               m_pStagingBuffer = { nullptr };
    ID3D11Buffer*               m_pCountStagingBuffer = { nullptr };

protected:
    VTXPARTICLE*    m_pInstanceVertices = { nullptr };
    VTXPARTICLEGPU* m_pInstanceVerticesGPU = { nullptr };
    _uint           m_bIsLoop = { 0 };
    _uint           m_bIsVerticalFade = {};
    _uint           m_bIsVerticalShrink = {};
    _uint           m_bRadialRotation = {};
    _uint           m_bIsBillboard = {};
    _uint           m_iNumInstance = {};
    _uint           m_iMaxInstance = {};
    _uint           m_iInstanceVertexStride = {};
    _uint           m_iIndexCountPerInstance = {}; 
    _float*         m_pSpeeds = { nullptr };
    _float2         m_vSize = {};
    _float2         m_vSpeed = {};
    _float2         m_vRotation = {};
    _float2         m_vLifeTime = {};
    _float2         m_vStartDelay = {};
    _float3         m_vCenter = {};
    _float3         m_vRange = {};
    _float3         m_vPivot = {};
    _float          m_fAlpha = {};
    _float          m_fParticleScale = { 1.f };
    _int            m_iDirectionalScale = {};
    _float          m_fAccumulatedTime = {};                // 누적 시간
    _float          m_fShakeStrength = {};                  // 흔들림 강도
    _float          m_fConvergeMaxDist = {};

    _float*         m_pRotations = { nullptr };             // 회전값
    _float*         m_pRotationSpeeds = { nullptr };        // 회전 속도
    _float          m_fGravity = { };                       // 중력

    _float*         m_pAngles = { nullptr };                // 각도
    _float          m_fTransparency = { 1.f };              // 쉐이더에 던질 투명도

    // 스프라이트 이미지 변수
    _uint           m_iFrameCount = {};                     // 스프라이트 이미지 개수
    _uint           m_iCurrentFrameIndex = {};              // 현재 이미지 인덱스
    _uint           m_iCountX = {};                         // 가로 개수
    _uint           m_iCountY = {};                         // 세로 개수
    _uint           m_iTransparentIndex = {};               // 투명도 적용 시작할 인덱스
    _float          m_fSizeX = {};                          // 한 장의 가로 사이즈
    _float          m_fSizeY = {};                          // 한 장의 세로 사이즈
    _float          m_fFrameDelay = {};                     // 프레임 진행 속도

    // 마스크 스프라이트 이미지 변수
    _uint           m_iMaskStartFrame = {};
    _uint           m_iMaskFrameCount = {};
    _uint           m_iMaskCountX = {};
    _uint           m_iMaskCountY = {};
    _float2         m_vMaskUVScroll = {};

    // 위치 설정하는 오프셋
    _uint           m_bFollowParent = {};

    _float4x4       m_matWorldOffset = { 1.f, 0.f, 0.f, 0.f,
                                         0.f, 1.f, 0.f, 0.f,
                                         0.f, 0.f, 1.f, 0.f,
                                         0.f, 0.f, 0.f, 1.f};

    _float4x4       m_matPrevWorldOffset = { 1.f, 0.f, 0.f, 0.f,
                                             0.f, 1.f, 0.f, 0.f,
                                             0.f, 0.f, 1.f, 0.f,
                                             0.f, 0.f, 0.f, 1.f};

public:
    static VIBuffer_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual Component* Clone(void* arg) override;

public:
    void Free() override;

};

NS_END
