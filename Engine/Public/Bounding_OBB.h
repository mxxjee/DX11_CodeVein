#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class Bounding_OBB final : public Bounding
{
public:
    typedef struct tagBounding_OBBDesc : Bounding::BOUND_DESC {
        _float3			vExtents{};
        _float3			vAngles{};
    }BOUNDOBB_DESC;

private:
    explicit Bounding_OBB();
    explicit Bounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~Bounding_OBB();

public:
    HRESULT Initialize(BOUND_DESC* _boundesc);
    _int	Update_Priority(const _float fTimeDelta);
    _int Update(const _fmatrix& _worldmatrix) override final;
    _int	Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta, PrimitiveBatch<VertexPositionColor>* _batch, _bool _isColl) override final;

    const BoundingOrientedBox* Get_Desc() const { return m_pDesc; }
    const BoundingOrientedBox* As_OBB() const override { return m_pDesc; }

    _bool Intersects(Bounding* _other) override final;


private:
    BoundingOrientedBox* m_pOriginalDesc = { nullptr };
    BoundingOrientedBox* m_pDesc = { nullptr };

public:
    static Bounding_OBB* Create();
    static Bounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUND_DESC* _boundesc);

public:
    void Free() override final;
};

NS_END
