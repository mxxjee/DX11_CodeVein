#pragma once

#include "VIBuffer_Particle.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Particle_Point final : public VIBuffer_Particle
{
public:
    typedef struct tagVIBufferParticlePointDesc : public VIBuffer_Particle::PARTICLE_DESC {

    }PARTICLE_POINT_DESC;

private:
    explicit VIBuffer_Particle_Point();
    explicit VIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit VIBuffer_Particle_Point(const VIBuffer_Particle_Point& original);
    virtual ~VIBuffer_Particle_Point();

public:
    HRESULT Initialize_Prototype(const PARTICLE_DESC* _desc);
    HRESULT Initialize(void* arg);
    HRESULT Bind_Resource() override final;
    HRESULT Bind_EffectResources(class Shader* pShader);
    HRESULT Render(const _float fTimeDelta) override final;

public:
    static VIBuffer_Particle_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const PARTICLE_DESC* _desc);
    virtual Component* Clone(void* arg) override;

public:
    void Free() override final;

};

NS_END
