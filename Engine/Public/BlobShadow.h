#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL BlobShadow final : public Component
{
public:
    typedef struct tagShadowDesc
    {
        _uint iLevel = {};
        _float fShadowRadius = 1.5f;     // 그림자 반경
        _float fShadowIntensity = 0.5f;     // 그림자 강도 (0~1)
        _float fShadowSoftness = 0.3f;     // 가장자리 부드러움
        _float fYOffset = 0.05f;    // Z-fighting 방지용 Y 오프셋
    }SHADOW_DESC;

private:
    explicit BlobShadow();
    explicit BlobShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit BlobShadow(const BlobShadow& original);
    virtual ~BlobShadow();

public:
    HRESULT Initialize_Prototype();
    HRESULT Initialize(void* arg);
    HRESULT Render(const _float fTimeDelta);

public:
    // 그림자 파라미터 설정 함수
    void Set_Radius(_float fRadius) { m_fShadowRadius = fRadius; }
    void Set_Intensity(_float fIntensity) { m_fShadowIntensity = fIntensity; }
    void Set_Softness(_float fSoftness) { m_fShadowSoftness = fSoftness; }
    void Set_YOffset(_float fOffset) { m_fYOffset = fOffset; }

    // 소유자의 Transform 설정
    void Set_OwnerTransform(class Transform* pTransform) { m_pOwnerTransform = pTransform; }

private:
    class Shader* m_pShaderCom = { nullptr };
    class VIBuffer_Rect* m_pVIBufferCom = { nullptr };
    class Transform* m_pOwnerTransform = { nullptr };  // 소유자의 Transform (AddRef 안함)

    // 그림자 파라미터
    _float m_fShadowRadius = 1.5f;     // 그림자 반경
    _float m_fShadowIntensity = 0.5f;     // 그림자 강도 (0~1)
    _float m_fShadowSoftness = 0.3f;     // 가장자리 부드러움
    _float m_fYOffset = 0.05f;    // Z-fighting 방지용 Y 오프셋

    _uint m_iLevel = {};

public:
    static BlobShadow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual Component* Clone(void* arg) override;

public:
    void Free() override final;
};

NS_END
