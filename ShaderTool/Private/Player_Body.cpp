#include "ShaderTool_Define.h"
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

	PLAYER_BODY_SHADER_DESC desc;

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

	Model* pMasterModelCom;
	for (int i = 0; i < 7; i++)
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
_int Player_Body::Update_Priority(const _float fTimeDelta)
{
	__super::Setup_CombinedWorldMatrix(XMLoadFloat4x4(m_matParentMatrix));

	return 0;
}

_int Player_Body::Update(const _float fTimeDelta)
{
	//여기서 다르면 패스 새로 넣어준다
	if (m_iPrevMeshNum != m_iBodyMeshNum)
	{
		if (m_iBodyMeshNum == 0 || m_iBodyMeshNum == 1)
		{
			Set_Pass_VecObjPass(0, 2); //스킨 
			Set_Pass_VecObjPass(1, 2); //스킨
			Set_Pass_VecObjPass(2, 3); //옷 메쉬
		}
		else if (m_iBodyMeshNum == 2 || m_iBodyMeshNum == 4)
		{
			Set_Pass_VecObjPass(0, 2); //스킨 
			Set_Pass_VecObjPass(1, 3); //옷
			Set_Pass_VecObjPass(2, 3); //옷
		}
		else if (m_iBodyMeshNum == 3 || m_iBodyMeshNum == 5 || m_iBodyMeshNum == 6)
		{
			Set_Pass_VecObjPass(0, 3); //옷
			Set_Pass_VecObjPass(1, 2); //스킨
			Set_Pass_VecObjPass(2, 2); //스킨
		}
	}
	m_iPrevMeshNum = m_iBodyMeshNum;
	m_iRimNoiseTime += fTimeDelta;
	return 0;
}

_int Player_Body::Update_Late(const _float fTimeDelta)
{
	GameObject::Add_RenderGroup(RENDER_GROUP::NONBLEND); //
	GameObject::Add_RenderGroup(RENDER_GROUP::SHADOW); //

	return 0;
}


HRESULT Player_Body::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	Model* pMasterRigModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);

	CHECK_FAILED(m_pRimNoiseTexture->Bind_ShaderResource_FullSlot(m_pShaderCom, 18, 0), E_FAIL);
	m_pShaderCom->Bind_RawValue_ByHandle(g_fTime, &m_iRimNoiseTime, sizeof(_float));

	for (_uint i = 0; i < m_pModelComVec[m_iBodyMeshNum]->Get_NumMeshes(); i++)
	{
		/* 쉐이더의 0번 패스를 사용해서 그리겠다고 선언 */
		CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);
		_uint bitflag = 0;
		//바디도 메쉬별로 바인딩 분류 해줘야하긴함.
		CHECK_FAILED(m_pModelComVec[m_iBodyMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL); //Diffuse
		CHECK_FAILED(m_pModelComVec[m_iBodyMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR, 0, &bitflag), E_FAIL); //S
		CHECK_FAILED(m_pModelComVec[m_iBodyMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &bitflag), E_FAIL);//N
		CHECK_FAILED(m_pModelComVec[m_iBodyMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_OPACITY, 0, &bitflag), E_FAIL); //ID
		CHECK_FAILED(m_pModelComVec[m_iBodyMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 6, i, aiTextureType_SHININESS, 0, &bitflag), E_FAIL);//White_Mask (거칠기?)
		CHECK_FAILED(m_pModelComVec[m_iBodyMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitflag), E_FAIL);//U

		m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));

		CHECK_FAILED(pMasterRigModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelComVec[m_iBodyMeshNum]->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Morphs(m_pShaderCom, i), E_FAIL);

		/* 0번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelComVec[m_iBodyMeshNum]->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT Player_Body::Ready_ShaderValue()
{
	//ID를 통해서 해당 색상에 섞을 값들 정의 Body_Female1 버전 

	m_tPlayerBodyShader.vInnerColor[0] = { 0.16155f,  0.175703f, 0.186978f, 1.0f };	//L1
	m_tPlayerBodyShader.vInnerColor[1] = { 0.2f,      0.206854f, 0.217878f, 1.0f };	//L2
	m_tPlayerBodyShader.vInnerColor[2] = { 0.52273f,  0.518272f, 0.505769f, 1.0f };	//L3
	m_tPlayerBodyShader.vInnerColor[3] = { 0.627669f, 0.660066f, 0.692708f, 1.0f };	//L4
	m_tPlayerBodyShader.vInnerColor[4] = { 0.253648f, 0.27443f,  0.278365f, 1.0f };	//L5
	m_tPlayerBodyShader.vInnerColor[5] = { 0.632473f, 0.0f,      0.222074f, 1.0f };;	//L6

	m_vInnerColor.resize(8, vector<_float4>(6));

	m_vInnerColor[0][0] = { 0.16155f,  0.175703f, 0.186978f, 1.0f };	//L1
	m_vInnerColor[0][1] = { 0.2f,      0.206854f, 0.217878f, 1.0f };	//L2
	m_vInnerColor[0][2] = { 0.52273f,  0.518272f, 0.505769f, 1.0f };	//L3
	m_vInnerColor[0][3] = { 0.627669f, 0.660066f, 0.692708f, 1.0f };	//L4
	m_vInnerColor[0][4] = { 0.253648f, 0.27443f,  0.278365f, 1.0f };	//L5
	m_vInnerColor[0][5] = { 0.632473f, 0.0f,      0.222074f, 1.0f };;	//L6

	m_vInnerColor[1][0] = { 1.0f,      0.753875f, 0.405f,    1.0f };	//L1
	m_vInnerColor[1][1] = { 0.875f,    0.767166f, 0.695464f, 1.0f };	//L2
	m_vInnerColor[1][2] = { 0.32f,     0.251011f, 0.152602f, 1.0f };	//L3
	m_vInnerColor[1][3] = { 0.349733f, 0.180564f, 0.632473f, 1.0f };	//L4
	m_vInnerColor[1][4] = { 0.150349f, 0.162668f, 0.165f,    1.0f };	//L5
	m_vInnerColor[1][5] = { 0.45f,     0.255188f, 0.045f,    1.0f };	//L6

	m_vInnerColor[2][0] = { 0.354492f, 0.543818f, 0.630208f, 1.0f };	//L1
	m_vInnerColor[2][1] = { 0.885478f, 0.9f,      0.729f,    1.0f };	//L2
	m_vInnerColor[2][2] = { 0.2f,      0.206854f, 0.217878f, 1.0f };	//L3
	m_vInnerColor[2][3] = { 0.63f,     0.56549f,  0.47892f,  1.0f };	//L4
	m_vInnerColor[2][4] = { 0.911207f, 0.847184f, 0.0f,      1.0f };	//L5
	m_vInnerColor[2][5] = { 0.555767f, 0.2145f,   0.715f,    1.0f };	//L6

	m_vInnerColor[3][0] = { 0.531014f, 0.512056f, 0.572917f, 1.0f };	//L1
	m_vInnerColor[3][1] = { 0.260417f, 0.245261f, 0.242784f, 1.0f };	//L2
	m_vInnerColor[3][2] = { 0.286458f, 0.085291f, 0.193505f, 1.0f };	//L3
	m_vInnerColor[3][3] = { 0.260417f, 0.245261f, 0.242784f, 1.0f };	//L4
	m_vInnerColor[3][4] = { 0.140625f, 0.04187f,  0.094993f, 1.0f };	//L5
	m_vInnerColor[3][5] = { 0.260417f, 0.245261f, 0.242784f, 1.0f };	//L6

	m_vInnerColor[4][0] = { 0.130051f, 0.146495f, 0.151042f, 1.0f };	//L1
	m_vInnerColor[4][1] = { 0.203369f, 0.328125f, 0.262774f, 1.0f };	//L2
	m_vInnerColor[4][2] = { 0.13425f,  0.200715f, 0.255208f, 1.0f };	//L3
	m_vInnerColor[4][3] = { 0.6491f,   0.651042f, 0.617133f, 1.0f };	//L4
	m_vInnerColor[4][4] = { 0.193011f, 0.364583f, 0.350058f, 1.0f };	//L5
	m_vInnerColor[4][5] = { 0.200521f, 0.201699f, 0.25f,     1.0f };	//L6

	m_vInnerColor[5][0] = { 0.419651f, 0.435176f, 0.885417f, 1.0f };	//L1
	m_vInnerColor[5][1] = { 0.650256f, 0.638753f, 0.734375f, 1.0f };	//L2
	m_vInnerColor[5][2] = { 0.345911f, 0.254259f, 0.473958f, 1.0f };	//L3
	m_vInnerColor[5][3] = { 0.43814f,  0.293946f, 0.661458f, 1.0f };	//L4
	m_vInnerColor[5][4] = { 0.490463f, 0.317314f, 0.677083f, 1.0f };	//L5
	m_vInnerColor[5][5] = { 0.737304f, 0.754508f, 0.786458f, 1.0f };	//L6

	m_vInnerColor[6][0] = { 0.572917f, 0.057378f, 0.0f,      1.0f };	//L1
	m_vInnerColor[6][1] = { 0.129234f, 0.052115f, 0.229167f, 1.0f };	//L2
	m_vInnerColor[6][2] = { 0.187483f, 0.229167f, 0.182617f, 1.0f };	//L3
	m_vInnerColor[6][3] = { 0.067708f, 0.038439f, 0.065687f, 1.0f };	//L4
	m_vInnerColor[6][4] = { 0.182292f, 0.137668f, 0.171513f, 1.0f };	//L5
	m_vInnerColor[6][5] = { 0.14388f,  0.155501f, 0.177083f, 1.0f };	//L6

	m_tPlayerBodyShader.fColorStrength = 1.f;

	return S_OK;
}

HRESULT Player_Body::Ready_Components()
{
	m_pModelComVec.resize(10);
	m_BoneRemaps.resize(10);
	CHECK_FAILED(Add_Component(_uint(LEVEL::STATIC), L"Prototype_Component_Shader_VTXPlayerAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerBody_Base0"), Com_Model, RCAST(Component**)(&m_pModelComVec[0])), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerBody_Base1"), L"Model_Component1", RCAST(Component**)(&m_pModelComVec[1])), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerBody_Base2"), L"Model_Component2", RCAST(Component**)(&m_pModelComVec[2])), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerBody_Base3"), L"Model_Component3", RCAST(Component**)(&m_pModelComVec[3])), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerBody_Base4"), L"Model_Component4", RCAST(Component**)(&m_pModelComVec[4])), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerBody_Base5"), L"Model_Component5", RCAST(Component**)(&m_pModelComVec[5])), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerBody_Base6"), L"Model_Component6", RCAST(Component**)(&m_pModelComVec[6])), E_FAIL);
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"RimNoise"), Com_Texture, RCAST(Component**)(&m_pRimNoiseTexture)), E_FAIL);

	Set_VecObjPassSize(10); //이건 넉넉히 주기(옷이 자꾸변함)
	Set_AllPass_VecObjPass(0); //0으로 초기화 

	Set_Pass_VecObjPass(0, 2); //스킨 
	Set_Pass_VecObjPass(1, 2); //스킨
	Set_Pass_VecObjPass(2, 3); //옷 메쉬

	//Set_Pass_VecObjPass(0, 2);
	//Set_Pass_VecObjPass(0, 2);
	//Set_Pass_VecObjPass(0, 2);
	//Set_Pass_VecObjPass(0, 2);s

	return S_OK;
}
HRESULT Player_Body::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_OBJECT, "g_WorldMatrix", m_matCombinedMatrix), E_FAIL);

	if (FAILED(m_pShaderCom->Begin(1, true)))
		return E_FAIL;

	Model* pMasterRigModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
	_uint   iNumMeshes = m_pModelComVec[m_iBodyMeshNum]->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		CHECK_FAILED(pMasterRigModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelComVec[m_iBodyMeshNum]->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		CHECK_FAILED(m_pShaderCom->Bind_Resources(1), E_FAIL);

		CHECK_FAILED(m_pModelComVec[m_iBodyMeshNum]->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}

HRESULT Player_Body::Bind_ShaderResources()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_matCombinedMatrix), E_FAIL);
	//CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);
	//CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

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