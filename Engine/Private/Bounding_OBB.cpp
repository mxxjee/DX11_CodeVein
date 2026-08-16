#include "Engine_Define.h"
#include "Bounding_OBB.h"

#include "DebugDraw.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Bounding_OBB::Bounding_OBB()
{
}

Engine::Bounding_OBB::Bounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Bounding(pDevice, pContext)
{
}

Engine::Bounding_OBB::~Bounding_OBB()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Bounding_OBB::Initialize(BOUND_DESC* _boundesc)
{
    BOUNDOBB_DESC* desc = CAST(BOUNDOBB_DESC*)(_boundesc);

    _float4     vRotation = {};
    XMStoreFloat4(&vRotation, 
        XMQuaternionRotationRollPitchYaw(XMConvertToRadians(desc->vAngles.x), XMConvertToRadians(desc->vAngles.y), XMConvertToRadians(desc->vAngles.z)));

    m_pOriginalDesc = new BoundingOrientedBox(desc->vCenter, desc->vExtents, vRotation);
    m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Bounding_OBB::Update_Priority(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Bounding_OBB::Update(const _fmatrix& _worldmatrix)
{
    m_pOriginalDesc->Transform(*m_pDesc, _worldmatrix);

    return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Bounding_OBB::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Bounding_OBB::Render(const _float fTimeDelta, PrimitiveBatch<VertexPositionColor>* _batch, _bool _isColl)
{
    DX::Draw(_batch, *m_pDesc, _isColl == true ? DirectX::Colors::HotPink : DirectX::Colors::Gold);

    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 충돌 검사 함수 ////////////////////////////////////////////////////////
_bool Engine::Bounding_OBB::Intersects(Bounding* _other)
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
Bounding_OBB* Engine::Bounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUND_DESC* _boundesc)
{
    Bounding_OBB* pInstance = new Bounding_OBB(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(_boundesc), L"Bounding_OBB 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Bounding_OBB::Free()
{
    __super::Free();

    Safe_Delete(m_pOriginalDesc);
    Safe_Delete(m_pDesc);
}
/******************************************************* 객체 반환 함수 *******************************************************/

