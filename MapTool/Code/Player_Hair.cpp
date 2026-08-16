#include "MT_Defines.h"
#include "Player_Hair.h"
#include "Model.h"
#include "Shader.h"

Player_Hair::Player_Hair()
{
}

Player_Hair::Player_Hair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice, pContext)
{
}

Player_Hair::Player_Hair(const Player_Hair& original)
	:PartObject(original)
{
}

Player_Hair::~Player_Hair()
{
}
HRESULT Player_Hair::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;

}

HRESULT Player_Hair::Initialize(void* arg)
{
	static _uint namenum = 0;

	HAIR_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(HAIR_DESC*)(arg)->wstrName = L"Player_Hair" + to_wstring(namenum++);

	CHECK_FAILED(PartObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	CHECK_FAILED(__super::RemapBones(m_pModelCom), E_FAIL);

	int failCount = 0;
	for (size_t i = 0; i < m_BoneRemap.size(); ++i)
	{
		if (m_BoneRemap[i] == g_INVALID)
			++failCount;
	}
	cout << "Hair Remap Fail Count : " << failCount << " / " << m_BoneRemap.size() << endl;

	Model* pMasterModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
	pMasterModelCom->RegisterPartModelCom(m_pModelCom);

	m_pModelCom->Ready_ReampSRV(pMasterModelCom, m_BoneRemap);

	return S_OK;
}

_int Player_Hair::Update_Priority(const _float fTimeDelta)
{
	__super::Setup_CombinedWorldMatrix(XMLoadFloat4x4(m_matParentMatrix));

	return 0;
}

_int Player_Hair::Update(const _float fTimeDelta)
{

	return 0;
}

_int Player_Hair::Update_Late(const _float fTimeDelta)
{

	GameObject::Add_RenderGroup(RENDER_GROUP::NONBLEND);

	return 0;
}

HRESULT Player_Hair::Render(const _float fTimeDelta)
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

		/* 0번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(0), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT Player_Hair::Ready_Components()
{
	CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_Shader_VTXPlayerAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerHair_Base"), Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);

	return S_OK;
}
HRESULT Player_Hair::Bind_ShaderResources()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_OBJECT, "g_WorldMatrix", m_matCombinedMatrix), E_FAIL);
	//CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);
	CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}

Player_Hair* Player_Hair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Player_Hair* pInstance = new Player_Hair(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Player_Hair 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Player_Hair::Clone(void* pArg)
{
	Player_Hair* pInstance = new Player_Hair(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Player_Hair 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Player_Hair::Free()
{
	__super::Free();
}