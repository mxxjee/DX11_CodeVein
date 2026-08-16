#include "Engine_Define.h"
#include "PartObject.h"
#include "Bone.h"
#include "Model.h"

#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::PartObject::PartObject()
{
}

Engine::PartObject::PartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Character(pDevice, pContext)
{
    m_bIsPartObj = true;
}

Engine::PartObject::PartObject(const PartObject& original)
    : Character(original)
{
    m_bIsPartObj = true;
}

Engine::PartObject::~PartObject()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::PartObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Engine::PartObject::Initialize(void* arg)
{
    PARTOBJECT_DESC* desc = CAST(PARTOBJECT_DESC*)(arg);

    m_matParentMatrix = desc->pParentMatrix;
    m_pMasterRig = desc->pMasterRig;


    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::PartObject::Update_Priority(const _float fTimeDelta)
{
    return 0;
}
_int Engine::PartObject::Update_Parallel(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::PartObject::Update(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::PartObject::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::PartObject::Render(const _float fTimeDelta)
{
    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 컴바인 함수 ////////////////////////////////////////////////////////
void Engine::PartObject::Setup_CombinedWorldMatrix(_fmatrix _parentMatrix)
{
    XMStoreFloat4x4(&m_matCombinedMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * _parentMatrix);
}
/******************************************************* 컴바인 함수 *******************************************************/

HRESULT Engine::PartObject::RemapBones(Model* pPartModelCom)
{
    vector<Bone*>& PartBone = pPartModelCom->Get_Bones();

    //m_BoneRemap.resize(PartBone.size()); // 뼈 개수만큼

    m_BoneRemap.assign(PartBone.size(), g_INVALID);

    Model* pMasterRigModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));

    for (_uint i = 0; i < m_BoneRemap.size(); ++i)
    {
        _string BoneName = PartBone[i]->Get_BoneName(); //이게 이제 파트 뼈 순서대로 이름이고
        _int iBoneIndex = pMasterRigModelCom->Get_BoneIndex(BoneName.c_str()); //마스터본의 뼈를 순회하면서 이름 비교해서 이름이 같은 뼈의 인덱스를 알수있고

        if (iBoneIndex < 0) //끝가지 찾지 못했을때 (-1반환)
        {
            continue;
        }
        else
        {
            m_BoneRemap[i] = (_uint)iBoneIndex; //여기에 이제 같은 이름뼈의 인덱스를 저장하고 
        }

    }

    for (_uint i = 0; i < m_BoneRemap.size(); ++i)
    {
        if (m_BoneRemap[i] == g_INVALID)
        {
            COUT("[REMFAIL] PartBoneIndex=" << i
                << " Name=" << PartBone[i]->Get_BoneName()
                << " Parent=" << PartBone[i]->Get_ParentBoneIndex()
                );
        }
    }

    return S_OK;
    
}

HRESULT Engine::PartObject::RemapBonesVec(Model* pPartModelCom, _int iNum)
{
    vector<Bone*>& PartBone = pPartModelCom->Get_Bones();

    //m_BoneRemap.resize(PartBone.size()); // 뼈 개수만큼

    m_BoneRemaps[iNum].assign(PartBone.size(), g_INVALID);

    Model* pMasterRigModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));

    for (_uint i = 0; i < m_BoneRemaps[iNum].size(); ++i)
    {
        _string BoneName = PartBone[i]->Get_BoneName(); //이게 이제 파트 뼈 순서대로 이름이고
        _int iBoneIndex = pMasterRigModelCom->Get_BoneIndex(BoneName.c_str()); //마스터본의 뼈를 순회하면서 이름 비교해서 이름이 같은 뼈의 인덱스를 알수있고

        if (iBoneIndex < 0) //끝가지 찾지 못했을때 (-1반환)
        {
            continue;
        }
        else
        {
            m_BoneRemaps[iNum][i] = (_uint)iBoneIndex; //여기에 이제 같은 이름뼈의 인덱스를 저장하고 
        }

    }

    for (_uint i = 0; i < m_BoneRemaps[iNum].size(); ++i)
    {
        if (m_BoneRemaps[iNum][i] == g_INVALID)
        {
            COUT("[REMFAIL] PartBoneIndex=" << i
                << " Name=" << PartBone[i]->Get_BoneName()
                << " Parent=" << PartBone[i]->Get_ParentBoneIndex()
            );
        }
    }

    return S_OK;

}

void Engine::PartObject::Setup_MasterModel()
{
    m_pMasterModelCom = CAST(Model*)(m_pMasterRig->Get_Component_FromName(Com_Model));
    MSG_NULL(m_pMasterModelCom, L"마스터 모델을 얻어오는데 실패했습니다", L"아오젠장", );
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
PartObject* Engine::PartObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return nullptr;
}

GameObject* Engine::PartObject::Clone(void* arg)
{
    return nullptr;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::PartObject::Free()
{
    __super::Free();

}
/******************************************************* 객체 반환 함수 *******************************************************/

