#include "MT_Defines.h"
#include "Player_Body.h"
#include "Model.h"
#include "Shader.h"
#include "Bone.h"
#include "Player_MasterRig.h"

Player_Body::Player_Body()
{
}

Player_Body::Player_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice, pContext)
{
}

Player_Body::Player_Body(const Player_Body& original)
	:PartObject(original)
{
}

Player_Body::~Player_Body()
{
}
HRESULT Player_Body::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;

}

HRESULT Player_Body::Initialize(void* arg)
{
	static _uint namenum = 0;

	BODY_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{
	
	}

	CAST(BODY_DESC*)(arg)->wstrName = L"Player_Body" + to_wstring(namenum++);

	CHECK_FAILED(PartObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	CHECK_FAILED(__super::RemapBones(m_pModelCom), E_FAIL);

	int failCount = 0;
	for (size_t i = 0; i < m_BoneRemap.size(); ++i)
	{
		if (m_BoneRemap[i] == g_INVALID)
			++failCount;
	}
	cout << "Body Remap Fail Count : " << failCount << " / " << m_BoneRemap.size() << endl;

	vector<Bone*> MyBones = m_pModelCom->Get_Bones();

	for (int i = 0; i < MyBones.size(); ++i)
	{
		if(MyBones[i]->Get_ParentBoneIndex()== 2)
			cout << "[ROOT] " << i << " : " << MyBones[i]->Get_BoneName() << endl;
	}

	Model* pMasterModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
	pMasterModelCom->RegisterPartModelCom(m_pModelCom);

	m_pModelCom->Ready_ReampSRV(pMasterModelCom, m_BoneRemap);

	return S_OK;
}

_int Player_Body::Update_Priority(const _float fTimeDelta)
{
	__super::Setup_CombinedWorldMatrix(XMLoadFloat4x4(m_matParentMatrix));

	return 0;
}

_int Player_Body::Update(const _float fTimeDelta)
{

	return 0;
}

_int Player_Body::Update_Late(const _float fTimeDelta)
{
	GameObject::Add_RenderGroup(RENDER_GROUP::NONBLEND); //

	return 0;
}
  

HRESULT Player_Body::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	/* 쉐이더의 0번 패스를 사용해서 그리겠다고 선언 */
	CHECK_FAILED(m_pShaderCom->Begin(0), E_FAIL);

	Model* pMasterRigModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE), E_FAIL);

		CHECK_FAILED(pMasterRigModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ShadCTexture", i, aiTextureType_EMISSIVE), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);


		//CHECK_FAILED(m_pModelCom->Bind_Morphs(m_pShaderCom, i), E_FAIL);

		/* 0번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(0), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT Player_Body::Ready_Components()
{
	CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_Shader_VTXPlayerAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerBody_Base"), Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);

	return S_OK;
}
HRESULT Player_Body::Bind_ShaderResources()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_OBJECT, "g_WorldMatrix", m_matCombinedMatrix), E_FAIL);
	//CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);
	CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}

Player_Body* Player_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Player_Body* pInstance = new Player_Body(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Player_Body 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Player_Body::Clone(void* pArg)
{
	Player_Body* pInstance = new Player_Body(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Player_Body 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Player_Body::Free()
{
	__super::Free();
}