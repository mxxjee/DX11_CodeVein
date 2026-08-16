#include "Engine_Define.h"
#include "Collider.h"

#include "GameInstance.h"
#include "Bounding_Sphere.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Collider::Collider()
{
}

Engine::Collider::Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Component(pDevice, pContext)
{
}

Engine::Collider::Collider(const Collider& original)
    : Component(original)
    , m_eType { original.m_eType }
#ifdef _DEBUG
    //, m_pBatch{ original.m_pBatch }
    //, m_pEffect{ original.m_pEffect }
    //, m_pInputLayout{ original.m_pInputLayout }
#endif
{
#ifdef _DEBUG
    //Safe_AddRef(m_pInputLayout);
    //m_pBatch = m_pGameInstance->Get_DebugBatch();
#endif // _DEBUG
}

Engine::Collider::~Collider()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Collider::Initialize_Prototype(COLLIDER _type)
{
    m_eType = _type;

#ifdef _DEBUG
    //m_pBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pContext);
    //m_pEffect = new BasicEffect(m_pDevice);
    //m_pEffect->SetVertexColorEnabled(true);

    //const void* shaderInput = {};
    //size_t shaderInputLength = {};

    //m_pEffect->GetVertexShaderBytecode(&shaderInput, &shaderInputLength);

    //MSG_FAIL(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, shaderInput, shaderInputLength, &m_pInputLayout),
    //    L"Collier의 InputLayout 생성에 실패했습니다!", L"디버그 생성 실패", E_FAIL);
#endif

    return S_OK;
}

HRESULT Engine::Collider::Initialize(void* _arg)
{
    Bounding::BOUND_DESC* desc = CAST(Bounding::BOUND_DESC*)(_arg);

    switch(m_eType)
    {
    case COLLIDER::SPHERE:
        m_pBounding = Bounding_Sphere::Create(m_pDevice, m_pContext, desc);
        break;
    case COLLIDER::AABB:
        m_pBounding = Bounding_AABB::Create(m_pDevice, m_pContext, desc);
        break;
    case COLLIDER::OBB:
        m_pBounding = Bounding_OBB::Create(m_pDevice, m_pContext, desc);
        break;
    }


    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Collider::Update_Priority(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Collider::Update(const _fmatrix& _wordMatrix)
{
    return m_pBounding->Update(_wordMatrix);
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Collider::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Collider::Render(const _float fTimeDelta)
{
#ifdef _DEBUG
#endif // _DEBUG

    return S_OK;
}

#ifdef _DEBUG
void Engine::Collider::Render_Debug(PrimitiveBatch<DirectX::VertexPositionColor>* _batch)
{
    m_pBounding->Render(0.f, _batch, m_bIsColl);
}

void Engine::Collider::Add_Debug_Render()
{
    m_pGameInstance->Add_DebugComponent(this);
}
#endif // _DEBUG

/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Collider* Engine::Collider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER _type)
{
    Collider* pInstance = new Collider(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(_type), L"Collider 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Collider* Engine::Collider::Clone(void* arg)
{
    Collider* pInstance = new Collider(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"Collider 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Collider::Free()
{
    __super::Free();

#ifdef _DEBUG
    //if (m_bIsClone == false)
    //{
    //    Safe_Delete(m_pBatch);
    //    Safe_Delete(m_pEffect);
    //}

    //Safe_Release(m_pInputLayout);
#endif

    Safe_Release(m_pBounding);
}
/******************************************************* 객체 반환 함수 *******************************************************/

