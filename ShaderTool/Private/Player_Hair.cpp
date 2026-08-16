#include "ShaderTool_Define.h"
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

	Model* pMasterModelCom;
	for (int i = 0; i < 5; i++)
	{
		CHECK_FAILED(__super::RemapBonesVec(m_pModelComVec[i], i), E_FAIL);

		int failCount = 0;
		for (size_t j = 0; j < m_BoneRemaps[i].size(); ++j)
		{
			if (m_BoneRemaps[i][j] == g_INVALID)
				++failCount;
		}

		pMasterModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
		pMasterModelCom->RegisterPartModelCom(m_pModelComVec[i]);

		m_pModelComVec[i]->Ready_ReampSRV(pMasterModelCom, m_BoneRemaps[i]);
	}


	CHECK_FAILED(Ready_ShaderValue(), E_FAIL);

	return S_OK;
}

_int Player_Hair::Update_Priority(const _float fTimeDelta)
{
	__super::Setup_CombinedWorldMatrix(XMLoadFloat4x4(m_matParentMatrix));

	return 0;
}

_int Player_Hair::Update(const _float fTimeDelta)
{
	m_iRimNoiseTime += fTimeDelta;
	return 0;
}

_int Player_Hair::Update_Late(const _float fTimeDelta)
{

	GameObject::Add_RenderGroup(RENDER_GROUP::NONBLEND);
	GameObject::Add_RenderGroup(RENDER_GROUP::SHADOW); //

	return 0;
}

HRESULT Player_Hair::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	Model* pMasterRigModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Begin(4), E_FAIL);

	CHECK_FAILED(m_pRimNoiseTexture->Bind_ShaderResource_FullSlot(m_pShaderCom, 18, 0), E_FAIL);
	m_pShaderCom->Bind_RawValue_ByHandle(g_fTime, &m_iRimNoiseTime, sizeof(_float));

	for (_uint i = 0; i < m_pModelComVec[m_iHairMeshNum]->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE), E_FAIL);
		CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR), E_FAIL);
		CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE), E_FAIL);
		CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS), E_FAIL);
		CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS), E_FAIL); //Metalness
		CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 6, i, aiTextureType_SHININESS), E_FAIL);//White_Mask (거칠기?) / Roughness

		CHECK_FAILED(pMasterRigModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		/* 0번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(4), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT Player_Hair::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_OBJECT, "g_WorldMatrix", m_matCombinedMatrix), E_FAIL);

	if (FAILED(m_pShaderCom->Begin(1, true)))
		return E_FAIL;

	Model* pMasterRigModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
	_uint   iNumMeshes = m_pModelComVec[m_iHairMeshNum]->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		CHECK_FAILED(pMasterRigModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		CHECK_FAILED(m_pShaderCom->Bind_Resources(1), E_FAIL);

		CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}

HRESULT Player_Hair::Ready_ShaderValue()
{
	//이거는 Hair_Female1 정보 
	m_tPlayerHairShader.fPlayerHighlight1Shift = -1.17f;
	m_tPlayerHairShader.fPlayerHighlight1Strength = 0.03f;
	m_tPlayerHairShader.fPlayerHighlight2Shift = -0.82f;
	m_tPlayerHairShader.fPlayerHighlight2Strength = 0.5f;
	m_tPlayerHairShader.fPlayerHighlightPower = 40.0f;
	m_tPlayerHairShader.fPlayerMetallic = 0.1f;
	m_tPlayerHairShader.fPlayerRoughnessMin = 0.5f;
	m_tPlayerHairShader.vPlayerHairColor = { 0.376f, 0.282f, 0.173f, 1.0f };

	m_iHairMeshNum = 0.f;
	return S_OK;
}

HRESULT Player_Hair::Ready_Components()
{
	m_pModelComVec.resize(10);
	m_BoneRemaps.resize(10);
	CHECK_FAILED(Add_Component(_uint(LEVEL::STATIC), L"Prototype_Component_Shader_VTXPlayerAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerHair_Base0"), Com_Model, RCAST(Component**)(&m_pModelComVec[0])), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerHair_Base1"), L"Model_Component1", RCAST(Component**)(&m_pModelComVec[1])), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerHair_Base2"), L"Model_Component2", RCAST(Component**)(&m_pModelComVec[2])), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerHair_Base3"), L"Model_Component3", RCAST(Component**)(&m_pModelComVec[3])), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerHair_Base4"), L"Model_Component4", RCAST(Component**)(&m_pModelComVec[4])), E_FAIL);
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"RimNoise"), Com_Texture, RCAST(Component**)(&m_pRimNoiseTexture)), E_FAIL);

	//Set_VecObjPassSize(m_pModelComVec[0]->Get_NumMeshes()); //사이즈 잡고 
	//Set_AllPass_VecObjPass(0); //0으로 초기화 

	//Set_Pass_VecObjPass(0, 4); //머리는 메쉬 하나  / 4번 패스 사용 

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