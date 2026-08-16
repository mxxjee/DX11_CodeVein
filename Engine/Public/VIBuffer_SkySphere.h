// VIBuffer_SkySphere.h
#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_SkySphere final : public VIBuffer
{
private:
    explicit VIBuffer_SkySphere();
    explicit VIBuffer_SkySphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit VIBuffer_SkySphere(const VIBuffer_SkySphere& original);
    virtual ~VIBuffer_SkySphere();

public:
    HRESULT Initialize_Prototype(_uint iSliceCount = 32, _uint iStackCount = 16, _float fRadius = 1.f, _bool UseFullSphere = false);
    HRESULT Initialize(void* arg) override;

private:
    _uint m_iSliceCount = {};   // 가로 분할 수
    _uint m_iStackCount = {};   // 세로 분할 수
    _float m_fRadius = {};

public:
    static VIBuffer_SkySphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        _uint iSliceCount = 32, _uint iStackCount = 16, _float fRadius = 1.f, _bool UseFullSphere = false);
    virtual Component* Clone(void* arg) override;

public:
    void Free() override final;
};

NS_END
