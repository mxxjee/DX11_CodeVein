#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class VIBuffer_Particle;
class Shader;
class Texture;
class SubEmitter;

class ENGINE_DLL ParticleEffect abstract : public GameObject
{
public:
    typedef struct tagEffectDesc : public GameObject::GAMEOBJECT_DESC
    {
        GameObject* pOwner = { nullptr };
    }EFFECT_DESC;

protected:
    explicit ParticleEffect();
    explicit ParticleEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit ParticleEffect(const ParticleEffect& original);
    virtual ~ParticleEffect() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual _int    Update_Priority(const _float fTimeDelta) override;
    virtual _int    Update(const _float fTimeDelta) override;
    virtual _int    Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;

public:
    virtual HRESULT Ready_Components();

public:
    virtual _bool                       Get_IsMeshEffect() { return false; }
    virtual _bool                       Get_IsTrailEffect() { return false; }
    _bool                               Is_SubEmitterOnly() { return m_bIsSubEmitter; }
    _bool                               Get_UseEdgeGlow() { return m_bUseEdgeGlow; }
    _bool                               Get_EffectLoop() { return m_bEffectLoop; }
    _bool                               Get_BackCulling() { return m_bBackCulling; }

    SubEmitter*                         Create_SubEmitter();
    SubEmitter*                         Get_SubEmitter() { return m_pSubEmitter; }
    VIBuffer_Particle::PARTICLE_DESC    Get_ParticleDesc() { return m_pVIBufferCom->Get_ParticleDesc(); }
    _float3                             Get_EffectRotation() { return m_vRotation; }
    _float2                             Get_DiffuseUVScroll() { return m_vDiffuseUVScroll; }
    _float2                             Get_FadeInOut() { return m_vFadeInOut; }
    _string                             Get_TextureName() { return m_strTextureName; }
    _string                             Get_NoiseTextureName() { return m_strNoiseTextureName; }
    _string                             Get_AlphaMaskTextureName() { return m_strAlphaMaskTextureName; }
    _string                             Get_MaskTextureName() { return m_strMaskTextureName; }
    _string                             Get_ShaderTag() { return m_strShaderName; }
    _string                             Get_ShaderComputeTag() { return m_strComputeShaderName; }
    _float4                             Get_Color() { return m_vColor; }
    _int                                Get_PassIndex() { return m_iPassIndex; }
    _uint                               Get_IsEmissive() { return m_bEmissive; }
    _uint                               Get_MaskSampler() { return m_iMaskSampler; }
    _float&                             Get_DistortionStrength() { return m_fDistortionStrength; }
    _float&                             Get_GlowStrength() { return m_fGlowStrength; }
    _float                              Get_LifeTime() { return m_fLifeTime; }
    _float                              Get_MaskIntensity() { return m_fMaskIntensity; }
    _float                              Get_HotFactor() { return m_fHotColor; }
    _float3                             Get_HotColor() { return m_vHotColor; }
    _float2                             Get_EffectOffset() { return m_vEffectOffset; }

    void Set_EffectRotation(_float3 vRotation) { m_vRotation = vRotation;
                                                 m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z)); }
    void Set_DiffuseUVScroll(_float2 vDiffuseUVScroll) { m_vDiffuseUVScroll = vDiffuseUVScroll; }
    void Set_FadeInOut(_float2 vFadeInOut) { m_vFadeInOut = vFadeInOut; }
    void Set_MaskIntensity(_float fMaskIntensity) { m_fMaskIntensity = fMaskIntensity; }
    void Set_BackCulling(_bool bBackCulling) { m_bBackCulling = bBackCulling; }
    void Set_EffectLoop(_bool bEffectLoop) { m_bEffectLoop = bEffectLoop; }
    void Set_LifeTime(_float fEffectLifeTime) { m_fLifeTime = fEffectLifeTime; }
    void Set_DistortionStrength(_float fDistortionStrength) { m_fDistortionStrength = fDistortionStrength; }
    void Set_GlowStrength(_float fGlowStrength) { m_fGlowStrength = fGlowStrength; }
    void Set_SubEmitter(_bool isSubEmitter) { m_bIsSubEmitter = isSubEmitter; }
    void Set_Owner(GameObject* _pOwner) { m_pOwner = _pOwner; }
    void Set_PassIndex(_int iPassIndex) { m_iPassIndex = iPassIndex;  Set_Pass_VecObjPass(0, m_iPassIndex); }
    void Set_Color(_float4 vColor) { m_vColor = vColor; }
    void Set_EdgeGlow(_bool bEdgeGlow) { m_bUseEdgeGlow = bEdgeGlow; }
    void Set_MaskSampler(_int iMaskSampler) { m_iMaskSampler = iMaskSampler; }
    void Set_HotColor(_float3 vHotColor, _float fHotColor) { m_vHotColor = vHotColor; m_fHotColor = fHotColor; }
    void Set_FollowParent(_bool bFollowParent);
    void Set_IsEmissive(_bool bEmissive) { m_bEmissive = bEmissive; }
    void Set_EffectOffset(_float2 vEffectOffset) { m_vEffectOffset = vEffectOffset; }
    virtual void Set_WorldMatrix(const _float4x4& vWorldOffset);

public:
    virtual void Apply_WorldOffset();
    virtual void Reset_MeshSetting() {};
    void Reset_Particles();
    void Reset_Position(const _float3& vPosition);

protected:
    GameObject*                 m_pOwner = { nullptr };
    SubEmitter*                 m_pSubEmitter = { nullptr };

    _float4x4                   m_matWorldOffset = { 1.f, 0.f, 0.f, 0.f,
                                                     0.f, 1.f, 0.f, 0.f,
                                                     0.f, 0.f, 1.f, 0.f,
                                                     0.f, 0.f, 0.f, 1.f };
    _float3                     m_vRotation = {};
    _float2                     m_vDiffuseUVScroll = {};
    _float2                     m_vFadeInOut = {};
    _float2                     m_vEffectOffset = {};

    _float                      m_fLifeTime = { 1.f };
    _float                      m_fAccumulatedTime = {};

    VIBuffer_Particle*          m_pVIBufferCom = { nullptr };
    Shader*                     m_pShaderCom = { nullptr };
    NewTexture*                 m_pTextureCom = { nullptr };
    ComputeShader*              m_pComputeShaderCom = { nullptr };
    ID3D11Buffer*               m_pParticleBuffer = { nullptr };		// 실제 데이터가 저장되는 메모리 공간
    ID3D11Buffer*               m_pInitDataBuffer = { nullptr };
    ID3D11ShaderResourceView*   m_pInitDataSRV = { nullptr };		    // 읽기 전용			ex) VS에서 파티클 데이터 읽기, PS에서 텍스처 샘플링, CS에서 입력 데이터 읽기 등
    ID3D11UnorderedAccessView*  m_pParticleUAV = { nullptr };	        // 읽기/쓰기 가능		ex) CS에서 데이터 수정 (파티클 업데이트, 물리 시뮬레이션), PS에서 렌더 타겟 쓰기 등

    _string                     m_strTextureName = {};
    _string                     m_strNoiseTextureName = {};
    _string                     m_strMaskTextureName = {};
    _string                     m_strAlphaMaskTextureName = {};
    _string                     m_strShaderName = {};
    _string                     m_strComputeShaderName = {};
    _bool                       m_bEffectLoop = {};
    _bool                       m_bBackCulling = { true };
    _bool                       m_bIsSubEmitter = {};
    _bool                       m_bUseEdgeGlow = {};
    _uint                       m_bEmissive = {};
    _uint		                m_iTextureNum = {};
    _uint                       m_iNoiseTextureNum = {};
    _uint                       m_iMaskTextureNum = {};
    _uint                       m_iAlphaMaskTextureNum = {};
    _int                        m_iPassIndex = {};
    _uint                       m_iMaskSampler = {};
    _float                      m_fDistortionStrength = {};
    _float                      m_fMaskIntensity = { 1.f };
    _float                      m_fGlowStrength = {};
    _float4                     m_vColor = { 1.f, 1.f, 1.f, 1.f };
    _float3                     m_vHotColor = { 1.f, 1.f, 1.f };
    _float                      m_fHotColor = {};

public:
    void Free() override;
};

NS_END
