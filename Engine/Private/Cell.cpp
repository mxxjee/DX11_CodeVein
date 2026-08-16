#include "Engine_Define.h"
#include "Cell.h"

#include "VIBuffer_Cell.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Cell::Cell()
{
}

Engine::Cell::Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

Engine::Cell::Cell(const Cell& original)
    : m_pDevice(original.m_pDevice), m_pContext(original.m_pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

Engine::Cell::~Cell()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Cell::Initialize(const _float3* _points, _int _index)
{
    memcpy(m_vPoints, _points, sizeof(_float3) * _UINT(NAVPOINT::END));

    m_iIndex = _index;

    _vector _line = {};

    _line = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::B)]) - XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::A)]), 0.f));
    m_vNormals[_UINT(NAVLINE::AB)] = _float3(XMVectorGetZ(_line) * -1.f, 0.f, XMVectorGetX(_line));

    _line = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::C)]) - XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::B)]), 0.f));
    m_vNormals[_UINT(NAVLINE::BC)] = _float3(XMVectorGetZ(_line) * -1.f, 0.f, XMVectorGetX(_line));

    _line = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::A)]) - XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::C)]), 0.f));
    m_vNormals[_UINT(NAVLINE::CA)] = _float3(XMVectorGetZ(_line) * -1.f, 0.f, XMVectorGetX(_line));

#ifdef _DEBUG
    m_pVIBuffer = VIBuffer_Cell::Create(m_pDevice, m_pContext, _points);
    CHECK_NULLPTR(m_pVIBuffer);
#endif // _DEBUG


    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



HRESULT Engine::Cell::Render(const _float fTimeDelta)
{
#ifdef _DEBUG
    CHECK_FAILED(m_pVIBuffer->Bind_Resource(), E_FAIL);

    m_pVIBuffer->Render(fTimeDelta);
#endif

    return S_OK;
}


//////////////////////////////////////////////////////// 닿아있는 라인 검사 함수 ////////////////////////////////////////////////////////
_bool Engine::Cell::Compare(const _fvector _sourcePoint, const _fvector _destPoint)
{
    /*XMVectorEqual();*/
    if (true == XMVector3Equal(_sourcePoint, XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::A)])))
    {
        if (true == XMVector3Equal(_destPoint, XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::B)])))		
            return true;

        if (true == XMVector3Equal(_destPoint, XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::C)])))
            return true;	
    }

    if (true == XMVector3Equal(_sourcePoint, XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::B)])))
    {
        if (true == XMVector3Equal(_destPoint, XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::C)])))
            return true;

        if (true == XMVector3Equal(_destPoint, XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::A)])))
            return true;
    }

    if (true == XMVector3Equal(_sourcePoint, XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::C)])))
    {
        if (true == XMVector3Equal(_destPoint, XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::A)])))
            return true;

        if (true == XMVector3Equal(_destPoint, XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::B)])))
            return true;
    }

    return false;
}
/******************************************************* 닿아있는 라인 검사 함수 *******************************************************/



//////////////////////////////////////////////////////// 계산 함수 ////////////////////////////////////////////////////////
_bool Engine::Cell::IsIn(_fvector _resultPos, _int& _cellIndex)
{
    for (_uint i = 0; i < _UINT(NAVLINE::END); i++)
    {
        _vector dir = XMVector3Normalize(_resultPos - XMLoadFloat3(&m_vPoints[i]));

        if (XMVectorGetX(XMVector3Dot(dir, XMLoadFloat3(&m_vNormals[i]))) > 0)
        {
            _cellIndex = m_iNeighborIndices[i];

            return false;
        }
    }

    return true;
}

_float Engine::Cell::Compute_Height(_fvector _cellPos)
{
    _vector plane = XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::A)])
        , XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::B)])
        , XMLoadFloat3(&m_vPoints[_UINT(NAVPOINT::C)]));

    // ax + by + cz + d = 0
    // y = (-ax -cz -d) / b
    // plane = a, b, c, d
    // _cellpos = x, y, z

    return -((XMVectorGetX(plane) * XMVectorGetX(_cellPos)) + (XMVectorGetZ(plane) * XMVectorGetZ(_cellPos)) + XMVectorGetW(plane)) / XMVectorGetY(plane);
}
/******************************************************* 계산 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Cell* Engine::Cell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* _points, _int _index)
{
    Cell* pInstance = new Cell(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(_points, _index), L"Cell 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Cell::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
#ifdef _DEBUG
    Safe_Release(m_pVIBuffer);
#endif
}
/******************************************************* 객체 반환 함수 *******************************************************/

