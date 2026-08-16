#include "Engine_Define.h"
#include "Bounding_AABB.h"

#include "DebugDraw.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Bounding_AABB::Bounding_AABB()
{
}

Engine::Bounding_AABB::Bounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Bounding(pDevice, pContext)
{
}

Engine::Bounding_AABB::~Bounding_AABB()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Bounding_AABB::Initialize(BOUND_DESC* _boundesc)
{
    BOUNDAABB_DESC* desc = CAST(BOUNDAABB_DESC*)(_boundesc);    

    m_pOriginalDesc = new BoundingBox(desc->vCenter, desc->vExtents);
    m_pDesc = new BoundingBox(*m_pOriginalDesc);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Bounding_AABB::Update_Priority(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Bounding_AABB::Update(const _fmatrix& _worldmatrix)
{
    _matrix     TransformMatrix = _worldmatrix;

    TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(_worldmatrix.r[0]);
    TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(_worldmatrix.r[1]);
    TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(_worldmatrix.r[2]);
    
    m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);

    return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Bounding_AABB::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Bounding_AABB::Render(const _float fTimeDelta, PrimitiveBatch<VertexPositionColor>* _batch, _bool _isColl)
{
    DX::Draw(_batch, *m_pDesc, _isColl == true ? DirectX::Colors::HotPink : DirectX::Colors::Gold);

    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 충돌 검사 함수 ////////////////////////////////////////////////////////
_bool Engine::Bounding_AABB::Intersects(Bounding* _other)
{
    if (auto* sphere = _other->As_Sphere())
        return m_pDesc->Intersects(*sphere);
    if (auto* aabb = _other->As_AABB())
        return m_pDesc->Intersects(*aabb);
    if (auto* obb = _other->As_OBB())
        return m_pDesc->Intersects(*obb);

    return false;
}
/******************************************************* 충돌 검사 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Bounding_AABB* Engine::Bounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUND_DESC* _boundesc)
{
    Bounding_AABB* pInstance = new Bounding_AABB(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(_boundesc), L"Bounding_AABB 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Bounding_AABB::Free()
{
    __super::Free();

    Safe_Delete(m_pOriginalDesc);
    Safe_Delete(m_pDesc);
}
/******************************************************* 객체 반환 함수 *******************************************************/

