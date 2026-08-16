#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Cell final : public VIBuffer
{
private:
    explicit VIBuffer_Cell();
    explicit VIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit VIBuffer_Cell(const VIBuffer_Cell& original);
    virtual ~VIBuffer_Cell();

public:
    HRESULT Initialize_Prototype(const _float3* pPoints);
    HRESULT Initialize(void* arg) override;

public:
    static VIBuffer_Cell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
    virtual Component* Clone(void* arg) override;

public:
    void Free() override final;

};

NS_END
