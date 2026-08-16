#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class Bounding_AABB final : public Bounding
{
public:
    typedef struct tagBounding_AABBDesc : Bounding::BOUND_DESC {
        _float3			vExtents{};
    }BOUNDAABB_DESC;

private:
    explicit Bounding_AABB();
    explicit Bounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~Bounding_AABB();

public:
    HRESULT Initialize(BOUND_DESC* _boundesc);
    _int	Update_Priority(const _float fTimeDelta);
    _int    Update(const _fmatrix& _worldmatrix) override final;
    _int	Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta, PrimitiveBatch<VertexPositionColor>* _batch, _bool _isColl) override final;

    const BoundingBox* Get_Desc() const { return m_pDesc; }
    const BoundingBox* As_AABB() const override { return m_pDesc; }

    _bool Intersects(Bounding* _other) override final;

private:
    BoundingBox* m_pOriginalDesc = { nullptr };
    BoundingBox* m_pDesc = { nullptr };

public:
    static Bounding_AABB* Create();
    static Bounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUND_DESC* _boundesc);

public:
    void Free() override final;
};

NS_END
