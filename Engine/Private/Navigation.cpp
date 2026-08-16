#include "Engine_Define.h"
#include "Navigation.h"

#include "GameInstance.h"
#include "Cell.h"

const _float4x4* Navigation::m_pParentMatrix = { nullptr };

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Navigation::Navigation()
{
}

Engine::Navigation::Navigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Component(pDevice, pContext)
{
}

Engine::Navigation::Navigation(const Navigation& original)
    : Component(original)
    , m_iCellIndex(original.m_iCellIndex)
    , m_vecCells(original.m_vecCells)
    , m_iNumCells(original.m_iNumCells)
#ifdef _DEBUG
    , m_pShader(original.m_pShader)
#endif // _DEBUG

{
    for (auto& cell : m_vecCells)
        Safe_AddRef(cell);

#ifdef _DEBUG
    Safe_AddRef(m_pShader);
#endif //_DEBUG
}

Engine::Navigation::~Navigation()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Navigation::Initialize_Prototype(const _wstring& NavigationData)
{
    _ulong dwByte = {};
    HANDLE hFile = CreateFile(NavigationData.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == 0)
        return E_FAIL;

    _float3 vPoints[_UINT(NAVPOINT::END)] = {};

    while (true)
    {
        _bool success = ReadFile(hFile, vPoints, sizeof(_float3) * _UINT(NAVPOINT::END), &dwByte, nullptr);
        if (dwByte == 0)
            break;

        Cell* pCell = Cell::Create(m_pDevice, m_pContext, vPoints, (_int)m_vecCells.size());

        m_vecCells.push_back(pCell);
    }

    CHECK_FAILED(Setup_Neighbors(), E_FAIL);


#ifdef _DEBUG
    SHADERENTRY entry[2] = { "VS_MAIN", "PS_MAIN" };
    SHADERENTRIES entries;
    entries.pEntries = entry;
    entries.iNumpass = 1;
    m_pShader = Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Cell.hlsl", VTXPOS::Elements, VTXPOS::iNumElements, entries);
    CHECK_NULLPTR(m_pShader);
#endif // _DEBUG

    CloseHandle(hFile);

    return S_OK;
}

HRESULT Engine::Navigation::Initialize(void* arg)
{
    NAVIGATION_DESC* pdesc = CAST(NAVIGATION_DESC*)(arg);

    m_iCellIndex = pdesc->iCurrentCellIndex;
    if(pdesc->pParentMatrix) m_pParentMatrix = pdesc->pParentMatrix;

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/


#ifdef _DEBUG
//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Navigation::Render(const _float fTimeDelta)
{
    CHECK_FAILED(m_pShader->Bind_Matrix_FullSlot(1, "g_WorldMatrix", *m_pParentMatrix), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Bind_PipeLineMatrix_View(m_pShader), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Bind_PipeLineMatrix_Proj(m_pShader), E_FAIL);

    m_pShader->Begin(0);

    for (Cell* cell : m_vecCells)
    {
        cell->Render(fTimeDelta);
    }

    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/
#endif // _DEBUG


_bool Engine::Navigation::IsMove(_fvector _resultPos)
{
    if (m_iCellIndex == -1) return false;

    _vector cellPos = XMVector3TransformCoord(_resultPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix)));

    _int neighborCellIndex = {};
    if (m_vecCells[m_iCellIndex]->IsIn(cellPos, neighborCellIndex) == false)
    {
        // 내비게이션을 완전히 벗어났다면 false
        if (neighborCellIndex == -1)
        {
            return false;
        }

        while (true)
        {
            if (m_vecCells[neighborCellIndex]->IsIn(cellPos, neighborCellIndex) == true)
                break;

            if (neighborCellIndex == -1)
            {
                return false;
            }

        }

        m_iCellIndex = neighborCellIndex;

        return true;
    }

    return true;
}

_vector Engine::Navigation::SetUp_OnNavigation(const _fvector _worldPos)
{
    _vector cellPos = XMVector3TransformCoord(_worldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix)));

    cellPos = XMVectorSetY(cellPos, m_vecCells[m_iCellIndex]->Compute_Height(cellPos));

    return XMVector3TransformCoord(cellPos, XMLoadFloat4x4(m_pParentMatrix));
}



//////////////////////////////////////////////////////// 이웃 셀 지정 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Navigation::Setup_Neighbors()
{
    for (auto& srcCell : m_vecCells)
    {
        for (auto& destCell : m_vecCells)
        {
            if (srcCell == destCell) continue;

            if (true == destCell->Compare(srcCell->Get_Point(NAVPOINT::A), srcCell->Get_Point(NAVPOINT::B)))
                srcCell->Set_Neighbor(NAVLINE::AB, destCell);

            if (true == destCell->Compare(srcCell->Get_Point(NAVPOINT::B), srcCell->Get_Point(NAVPOINT::C)))
                srcCell->Set_Neighbor(NAVLINE::BC, destCell);

            if (true == destCell->Compare(srcCell->Get_Point(NAVPOINT::C), srcCell->Get_Point(NAVPOINT::A)))
                srcCell->Set_Neighbor(NAVLINE::CA, destCell);
        }
    }

    return S_OK;
}
/******************************************************* 이웃 셀 지정 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Navigation* Engine::Navigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& NavigationData)
{
    Navigation* pInstance = new Navigation(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(NavigationData), L"Navigation 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Navigation* Engine::Navigation::Clone(void* arg)
{
    Navigation* pInstance = new Navigation(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"Navigation 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Navigation::Free()
{
    __super::Free();

    for (auto& cell : m_vecCells)
        Safe_Release(cell);
    m_vecCells.clear();

#ifdef _DEBUG
    Safe_Release(m_pShader);
#endif // _DEBUG

}
/******************************************************* 객체 반환 함수 *******************************************************/

