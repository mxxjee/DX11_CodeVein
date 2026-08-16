#include "Model_gltf.h"

#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Model_gltf::Model_gltf()
{
}

Engine::Model_gltf::Model_gltf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Component(pDevice, pContext)
{
}

Engine::Model_gltf::Model_gltf(const Model_gltf& original)
    : Component(original), m_eModelType(original.m_eModelType), m_iNumMeshes(original.m_iNumMeshes), m_iNumMaterials(original.m_iNumMaterials)
    , m_iNumBones(original.m_iNumBones), m_iNumAnimations(original.m_iNumAnimations), m_iCurrentAnimationIndex(original.m_iCurrentAnimationIndex)
    , m_PreTransformMatrix(original.m_PreTransformMatrix)
    , m_vecMeshes(original.m_vecMeshes), m_vecMaterials(original.m_vecMaterials)//, m_vecBones(original.m_vecBones), m_vecAnimatinos(original.m_vecAnimatinos)
    , m_Importer(), m_pAIScene(nullptr)
{
}

Engine::Model_gltf::~Model_gltf()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Model_gltf::Initialize_Prototype(MODEL _modeltype, const _wstring& _filepath, const _fmatrix& _preTransformMatrix)
{
   

    return S_OK;
}

HRESULT Engine::Model_gltf::Initialize(void* arg)
{
    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Model_gltf* Engine::Model_gltf::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL _modeltype, const _wstring& _filepath, const _fmatrix& _preTransformMatrix)
{
    Model_gltf* pInstance = new Model_gltf(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(_modeltype, _filepath, _preTransformMatrix), L"Model_gltf 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Component* Engine::Model_gltf::Clone(void* arg)
{
    Model_gltf* pInstance = new Model_gltf(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"Model_gltf 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Model_gltf::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

