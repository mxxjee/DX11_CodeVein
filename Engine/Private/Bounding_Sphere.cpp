#include "Engine_Define.h"
#include "Bounding_Sphere.h"

#include "DebugDraw.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Bounding_Sphere::Bounding_Sphere()
{
}

Engine::Bounding_Sphere::Bounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Bounding(pDevice, pContext)
{
}

Engine::Bounding_Sphere::~Bounding_Sphere()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Bounding_Sphere::Initialize(BOUND_DESC* _boundesc)
{
    BOUNDSPHERE_DESC* desc = CAST(BOUNDSPHERE_DESC*)(_boundesc);

    m_pOriginalDesc = new BoundingSphere(desc->vCenter, desc->fRadius);
    m_pDesc = new BoundingSphere(*m_pOriginalDesc);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Bounding_Sphere::Update_Priority(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Bounding_Sphere::Update(const _fmatrix& _worldmatrix)
{
    m_pOriginalDesc->Transform(*m_pDesc, _worldmatrix);

    return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Bounding_Sphere::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Bounding_Sphere::Render(const _float fTimeDelta, PrimitiveBatch<VertexPositionColor>* _batch, _bool _isColl)
{
    DX::Draw(_batch, *m_pDesc, _isColl == true ? DirectX::Colors::Red : DirectX::Colors::Gold);

    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 충돌 검사 함수 ////////////////////////////////////////////////////////
_bool Engine::Bounding_Sphere::Intersects(Bounding* _other)
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
Bounding_Sphere* Engine::Bounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUND_DESC* _boundesc)
{
    Bounding_Sphere* pInstance = new Bounding_Sphere(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(_boundesc), L"Bounding_Sphere 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Bounding_Sphere::Free()
{
    __super::Free();

    Safe_Delete(m_pOriginalDesc);
    Safe_Delete(m_pDesc);
}
/******************************************************* 객체 반환 함수 *******************************************************/

