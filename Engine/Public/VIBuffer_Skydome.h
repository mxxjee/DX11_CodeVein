// VIBuffer_Skydome.h
#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Skydome final : public VIBuffer
{
private:
    explicit VIBuffer_Skydome();
    explicit VIBuffer_Skydome(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit VIBuffer_Skydome(const VIBuffer_Skydome& original);
    virtual ~VIBuffer_Skydome();

public:
    HRESULT Initialize_Prototype(_uint iSliceCount = 32, _uint iStackCount = 16, _float fRadius = 1.f);
    HRESULT Initialize(void* arg) override;

private:
    _uint m_iSliceCount = {};   // 가로 분할 수
    _uint m_iStackCount = {};   // 세로 분할 수
    _float m_fRadius = {};

public:
    static VIBuffer_Skydome* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        _uint iSliceCount = 32, _uint iStackCount = 16, _float fRadius = 1.f);
    virtual Component* Clone(void* arg) override;

public:
    void Free() override final;
};

NS_END
