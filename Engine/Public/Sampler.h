#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Sampler final : public Base
{
private:
    explicit Sampler();
    explicit Sampler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~Sampler() override;

public:
    HRESULT Initialize(const D3D11_SAMPLER_DESC& _desc, _uint _slot);

    // Shader에 바인딩
    HRESULT Bind_Shader(_uint _stageMask);
    
    ID3D11SamplerState* GetSamplerState() const noexcept { return m_pSampler; }


private:
    ID3D11SamplerState* m_pSampler = { nullptr };
    _uint               m_ibindSlot = {};  // 리플렉션에서 추출 or 수동 지정

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };

public:
    static Sampler* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const D3D11_SAMPLER_DESC& _desc, _uint _slot);

public:
    virtual void Free() override final;
};

NS_END