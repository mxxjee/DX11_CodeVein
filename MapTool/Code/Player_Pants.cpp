#include "MT_Defines.h"
#include "Player_Pants.h"
#include "Model.h"
#include "Shader.h"

Player_Pants::Player_Pants()
{
}

Player_Pants::Player_Pants(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice,pContext)
{
}

Player_Pants::Player_Pants(const Player_Pants& original)
	:PartObject(original)
{
}

Player_Pants::~Player_Pants()
{
}
HRESULT Player_Pants::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;

}

HRESULT Player_Pants::Initialize(void* arg)
{
	static _uint namenum = 0;

	PANTS_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(PANTS_DESC*)(arg)->wstrName = L"Player_Pants" + to_wstring(namenum++);

	CHECK_FAILED(PartObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	CHECK_FAILED(__super::RemapBones(m_pModelCom), E_FAIL);

	int failCount = 0;
	for (size_t i = 0; i < m_BoneRemap.size(); ++i)
	{
		if (m_BoneRemap[i] == g_INVALID)
			++failCount;
	}
	cout << "Pants Remap Fail Count : " << failCount << " / " << m_BoneRemap.size() << endl;

	return S_OK;
}

_int Player_Pants::Update_Priority(const _float fTimeDelta)
{
	__super::Setup_CombinedWorldMatrix(XMLoadFloat4x4(m_matParentMatrix));

	return 0;
}

_int Player_Pants::Update(const _float fTimeDelta)
{

	return 0;
}

_int Player_Pants::Update_Late(const _float fTimeDelta)
{

	GameObject::Add_RenderGroup(RENDER_GROUP::NONBLEND);

	return 0;
}

HRESULT Player_Pants::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	/* 쉐이더의 0번 패스를 사용해서 그리겠다고 선언 */
	CHECK_FAILED(m_pShaderCom->Begin(0), E_FAIL);

	vector<Bone*>& MasterRigBones = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model))->Get_Bones();
		CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint)), E_FAIL);
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ShadCTexture", i, aiTextureType_EMISSIVE), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);



		//CHECK_FAILED(m_pModelCom->Bind_Morphs(m_pShaderCom, i), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_BonesMasterRig(m_pShaderCom, "g_BonesMatrices", i, MasterRigBones, m_BoneRemap), E_FAIL);

		/* 0번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(0), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}
HRESULT Player_Pants::Ready_Components()
{
	CHECK_FAILED(Add_Component(_UINT(LEVEL::TOOL), ProtoName::Proto_Com_Shader_VTXPlayerAnimMesh, Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerPants_Base"), Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);

	return S_OK;
}
HRESULT Player_Pants::Bind_ShaderResources()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_OBJECT, "g_WorldMatrix", m_matCombinedMatrix), E_FAIL);
	//CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);
	CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}

Player_Pants* Player_Pants::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Player_Pants* pInstance = new Player_Pants(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Player_Pants 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Player_Pants::Clone(void* pArg)
{
	Player_Pants* pInstance = new Player_Pants(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Player_Pants 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Player_Pants::Free()
{
	__super::Free();
}