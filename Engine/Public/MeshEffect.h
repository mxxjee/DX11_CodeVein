#pragma once
#include "Engine_Define.h"
#include "ParticleEffect.h"

NS_BEGIN(Engine)

class ENGINE_DLL MeshEffect abstract : public ParticleEffect
{
public:
	typedef struct tagMeshEffectDesc : public ParticleEffect::EFFECT_DESC
	{
		_float3 vPosition = {};
		_float3 vScale = {1.f, 1.f, 1.f};
        _float fMeshScale = { 1.f };
		_float fLifeTime = { 1.f };
        _float fScaleDelay = {};
        _uint iScaleAxis = { 7 };
		_int   iRotateRight = {};
		_int   iRotateUp = {};
		_int   iRotateLook = {};

        // Main Texture
        _uint iFrameCount = { 1 };
        _uint iCountX = { 1 };
        _uint iCountY = { 1 };
        _float fDisplaceStrength = {};

        // Mask Texture
        _uint iMaskStartFrame = {};
        _uint iMaskFrameCount = { 1 };
        _uint iMaskCountX = { 1 };
        _uint iMaskCountY = { 1 };
        _float2 vMaskUVScroll = {};
        _float2 vMaskUVScale = { 1.f, 1.f };
        _float2 vMaskUVStartOffset = {};

        // Dissolve Texture
        _uint       bDirectionalDissolve = {};
        _uint       bDissolveFlipY = {};
        _float      fNoiseWeight = {};
        _float      fDissolveEdgeWidth = {};
        _float3     vDissolveEdgeColor = { 1.f, 1.f, 1.f };

        _bool       bAlignToPlayerLook = { false };
    }MESHEFFECT_DESC;

protected:
    explicit MeshEffect();
    explicit MeshEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit MeshEffect(const MeshEffect& original);
    virtual ~MeshEffect() = default;

public:
    virtual _bool       Get_IsMeshEffect() { return true; }
    MESHEFFECT_DESC     Get_MeshDesc();

    void                Set_MeshDesc(const MESHEFFECT_DESC& desc);
    void                Set_ScaleX(_float fScaleX) { m_vScale.x = fScaleX; }
    void                Set_ScaleY(_float fScaleY) { m_vScale.y = fScaleY; }
    void                Set_ScaleZ(_float fScaleZ) { m_vScale.z = fScaleZ; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_Priority(const _float fTimeDelta) override;
    virtual _int    Update(const _float fTimeDelta) override;
    virtual _int    Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;

public:
    virtual void Apply_WorldOffset() override;
    virtual void Reset_MeshSetting() override;

protected:
    class Model*        m_pModelCom = { nullptr };

    _float3             m_vPosition = {};
	_float3             m_vScale = { 1.f, 1.f, 1.f };
	_float              m_fMeshScale = { 1.f };
    _float              m_fScaleDelay = {};
	_uint               m_iScaleAxis = { 7 };       // 비트플래그: 1=Right, 2=Up, 4=Look (기본 7=전부)

    _float3             m_vAccumulatedRotation = {};
    _int                m_iRotateRight = {};
    _int                m_iRotateUp = {};
    _int                m_iRotateLook = {};
    _uint               m_iFrameCount = { 1 };
    _uint               m_iCountX = { 1 };
    _uint               m_iCountY = { 1 };
    _float              m_fDisplaceStrength = {};

    _uint               m_iMaskStartFrame = {};
    _uint               m_iMaskFrameCount = { 1 };
    _uint               m_iMaskCountX = { 1 };
    _uint               m_iMaskCountY = { 1 };
    _float2             m_vMaskUVScroll = {};
    _float2             m_vMaskUVScale = { 1.f, 1.f };
    _float2             m_vMaskUVStartOffset = {};

    _uint               m_bDirectionalDissolve = {};
    _uint               m_bDissolveFlipY = {};
    _float              m_fNoiseWeight = {};
    _float              m_fDissolveEdgeWidth = {};
    _float3             m_vDissolveEdgeColor = { 1.f, 1.f, 1.f };

    _bool       m_bAlignToPlayerLook = { false };

public:
    virtual void Free() override;
};

NS_END