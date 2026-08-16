#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Bounding abstract : public Base
{
public:
    typedef struct tagBoundingDesc {
        _float3 vCenter{};
    }BOUND_DESC;

protected:
    explicit Bounding();
    explicit Bounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~Bounding();

public:
    HRESULT Initialize();
    virtual _int Update(const _fmatrix& _worldmatrix) = 0;
    virtual HRESULT Render(const _float fTimeDelta, PrimitiveBatch<VertexPositionColor>* _batch, _bool _isColl) = 0;

public:
    virtual _bool Intersects(Bounding* _other) = 0; // 다형성 충돌 판정

    virtual const BoundingSphere* As_Sphere() const { return nullptr; }     // 타입 캐스팅용
    virtual const BoundingBox* As_AABB() const { return nullptr; }          // 타입 캐스팅용
    virtual const BoundingOrientedBox* As_OBB() const { return nullptr; }   // 타입 캐스팅용

protected:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };

public:
    static Bounding* Create();
    static Bounding* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
    void Free() override;

};

NS_END
