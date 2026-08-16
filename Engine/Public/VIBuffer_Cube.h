#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Cube final : public VIBuffer
{
private:
    explicit VIBuffer_Cube();
    explicit VIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit VIBuffer_Cube(const VIBuffer_Cube& original);
    virtual ~VIBuffer_Cube();

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;

public:
    static VIBuffer_Cube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual Component* Clone(void* arg) override;

public:
    void Free() override final;

};

NS_END
