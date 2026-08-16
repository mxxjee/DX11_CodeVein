#pragma once

#include "VIBuffer_Particle.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Particle_Rect final : public VIBuffer_Particle
{
public:
    typedef struct tagVIBufferParticleRectDesc : public VIBuffer_Particle::PARTICLE_DESC {

    }PARTICLE_RECT_DESC;

private:
    explicit VIBuffer_Particle_Rect();
    explicit VIBuffer_Particle_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit VIBuffer_Particle_Rect(const VIBuffer_Particle_Rect& original);
    virtual ~VIBuffer_Particle_Rect();

protected:
    HRESULT Create_ComputeBuffers();
    HRESULT Recreate_GPUBuffers() override;

public:
    HRESULT Initialize_Prototype(const PARTICLE_DESC* _desc);
    HRESULT Initialize(void* arg);
    virtual HRESULT Bind_EffectResources(class Shader* pShader) override;
    virtual HRESULT Bind_ComputeShaderResources(class ComputeShader* pCShader, const _float& fTimeDelta) override;

public:
    static VIBuffer_Particle_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const PARTICLE_DESC* _desc);
    virtual Component* Clone(void* arg) override;

public:
    void Free() override final;

};

NS_END
