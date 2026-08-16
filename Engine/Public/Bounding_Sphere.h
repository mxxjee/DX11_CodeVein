#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class Bounding_Sphere final : public Bounding
{
public:
    typedef struct tagBounding_SphereDesc : Bounding::BOUND_DESC {
        _float fRadius{};
    }BOUNDSPHERE_DESC;

private:
    explicit Bounding_Sphere();
    explicit Bounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~Bounding_Sphere();

public:
    HRESULT Initialize(BOUND_DESC* _boundesc);
    _int	Update_Priority(const _float fTimeDelta);
    _int Update(const _fmatrix& _worldmatrix) override final;
    _int	Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta, PrimitiveBatch<VertexPositionColor>* _batch, _bool _isColl) override final;

    const BoundingSphere* Get_Desc() const { return m_pDesc; }
    const BoundingSphere* As_Sphere() const override { return m_pDesc; }
    
    _bool Intersects(Bounding* _other) override final;

private:
    BoundingSphere* m_pOriginalDesc = { nullptr };
    BoundingSphere* m_pDesc = { nullptr };

public:
    static Bounding_Sphere* Create();
    static Bounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUND_DESC* _boundesc);

public:
    void Free() override final;
};

NS_END
