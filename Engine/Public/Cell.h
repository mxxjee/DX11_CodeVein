#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Cell final : public Base
{
private:
    explicit Cell();
    explicit Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Cell(const Cell& original);
    virtual ~Cell();

public:
    HRESULT Initialize(const _float3* _points, _int _index);

    HRESULT Render(const _float fTimeDelta);

public:
    _vector Get_Point(NAVPOINT _point) const {
        return XMLoadFloat3(&m_vPoints[_UINT(_point)]);
    }
    _uint Get_Index() { return m_iIndex; }

    void Set_Neighbor(NAVLINE _line, Cell* _neighbor) {
        m_iNeighborIndices[_UINT(_line)] = _neighbor->m_iIndex;
    }

public:
    _bool Compare(const _fvector _sourcePoint, const _fvector _destPoint);
    _bool IsIn(_fvector _resultPos, _int& _cellIndex);
    _float Compute_Height(_fvector _cellPos); // cellPos = 객체가 Cell의 로컬 좌표로 들어간 위치


private:
    _float3 m_vPoints[_UINT(NAVPOINT::END)]{};
    _float3 m_vNormals[_UINT(NAVLINE::END)]{};
    _int m_iNeighborIndices[_UINT(NAVLINE::END)] = { -1, -1, -1 };
    _uint m_iIndex = {};

#ifdef _DEBUG
    class VIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };

public:
    static Cell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* _points, _int _index);
    
public:
    void Free() override final;

};

NS_END
