#include "Client_Define.h"
#include "Player_Head.h"
#include "Model.h"
#include "Shader.h"

Player_Head::Player_Head()
{
}

Player_Head::Player_Head(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice, pContext)
{
}

Player_Head::Player_Head(const Player_Head& original)
	:PartObject(original)
{
}

Player_Head::~Player_Head()
{
}
HRESULT Player_Head::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;

}

HRESULT Player_Head::Initialize(void* arg)
{
	static _uint namenum = 0;

	HEAD_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(HEAD_DESC*)(arg)->wstrName = L"Player_Head" + to_wstring(namenum++);
	m_pDissolveTime = CAST(HEAD_DESC*)(arg)->pDissolveTime;

	CHECK_FAILED(PartObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	CHECK_FAILED(__super::RemapBones(m_pModelCom), E_FAIL);

	Model* pMasterModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
	pMasterModelCom->RegisterPartModelCom(m_pModelCom);

	m_pModelCom->Ready_ReampSRV(pMasterModelCom, m_BoneRemap);

	CHECK_FAILED(Ready_EyeShaderValue(), E_FAIL);
	CHECK_FAILED(Ready_BrowShaderValue(), E_FAIL);

	return S_OK;
}

_int Player_Head::Update_Priority(const _float fTimeDelta)
{
	return 0;
}

_int Player_Head::Update(const _float fTimeDelta)
{
	__super::Setup_CombinedWorldMatrix(XMLoadFloat4x4(m_matParentMatrix));
	return 0;
}

_int Player_Head::Update_Late(const _float fTimeDelta)
{
	GameObject::Add_RenderGroup(RENDER_GROUP::NONBLEND);
	GameObject::Add_RenderGroup(RENDER_GROUP::SHADOW); //

	return 0;
}

HRESULT Player_Head::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	Model* pMasterRigModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));

	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);

	// Noise Texture
	_uint bDissolving = (m_pDissolveTime && *m_pDissolveTime > 0.f);
	_uint bUseNoise = bDissolving ? 1 : 0;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseNoise, &bUseNoise, sizeof(_uint)), E_FAIL);
	if (bUseNoise)
	{
		_float fTime = *m_pDissolveTime;
		CHECK_FAILED(m_pNoiseTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 12, 0), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_fTimeElapsed, &fTime, sizeof(_float));
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);

		_uint bitFlag = 0;

		if (m_vecObjPass[i] == 6) //눈
		{
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitFlag), E_FAIL);
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &bitFlag), E_FAIL);
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 6, i, aiTextureType_SHININESS, 0, &bitFlag), E_FAIL);
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 7, i, aiTextureType_OPACITY, 0, &bitFlag), E_FAIL);
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitFlag), E_FAIL);
			CHECK_FAILED(m_pEyeWhiteTexture->Bind_ShaderResource_FullSlot(m_pShaderCom, 14, m_tHeadTextureChange.iEyeWhiteNum), E_FAIL); //g_PlayerBrowTexture
			CHECK_FAILED(m_pEyeDetailTexture->Bind_ShaderResource_FullSlot(m_pShaderCom, 15, m_tHeadTextureChange.iEyeDetailNum), E_FAIL); //g_PlayerBrowTexture
			CHECK_FAILED(m_pEyeHighlightTexture->Bind_ShaderResource_FullSlot(m_pShaderCom, 16, m_tHeadTextureChange.iEyeHighlightNum), E_FAIL); //g_PlayerBrowTexture
		}
		if (i == 4) //4번에 아이래쉬던져줌
		{
			CHECK_FAILED(m_pEyelashTexture->Bind_ShaderResource_FullSlot(m_pShaderCom, 13, m_tHeadTextureChange.iEyelashNum), E_FAIL); //g_PlayerBrowTexture
			bitFlag |= (1 << 13); //패스 안만들고 13번으로 처리
		}
		else //얼굴
		{
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitFlag), E_FAIL);
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR, 0, &bitFlag), E_FAIL);
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &bitFlag), E_FAIL);
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &bitFlag), E_FAIL);
			//CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 7, i, aiTextureType_SHININESS, 0, &bitFlag), E_FAIL);
			//CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 1, i, aiTextureType_OPACITY, 0, &bitFlag), E_FAIL);
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitFlag), E_FAIL);
			CHECK_FAILED(m_pBrowTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 11, m_tHeadTextureChange.iBrowNum), E_FAIL); //g_PlayerBrowTexture
			CHECK_FAILED(m_pTattoTexture->Bind_ShaderResource_FullSlot(m_pShaderCom, 17, m_tHeadTextureChange.iTattoPaintNum), E_FAIL); //g_PlayerBrowTexture
		}

		// 1 g_bitFlag
		m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitFlag, sizeof(_uint));

		CHECK_FAILED(pMasterRigModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Morphs(m_pShaderCom, i), E_FAIL);

		/* 0번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);
		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}
HRESULT Player_Head::Ready_EyeShaderValue()
{
	//fmodel에서 읽어온 값 
	m_tPlayerEyeShaderDesc.vPlayerEyeBaseColor = { 0.76f, 0.298f, 0.451f, 1.0f };
	m_tPlayerEyeShaderDesc.vPlayerEyeAppendColor = { 0.945f, 0.753f, 0.643f, 1.0f };

	m_tPlayerEyeShaderDesc.fPlayerEyeScale = 0.75f;
	m_tPlayerEyeShaderDesc.fPlayerEyeIndividualScaleOffset = 1.4f; //안씀

	m_tPlayerEyeShaderDesc.fPlayerEyeOffsetU = 0.f;
	m_tPlayerEyeShaderDesc.fPlayerEyeOffsetV = 0.04f;

	m_tPlayerEyeShaderDesc.fPlayerEyeHighlightStrength = 0.1f;

	m_tPlayerEyeShaderDesc.fPlayerTatooOnOff = 0.f;//타투 ON/OFF
	m_tPlayerEyeShaderDesc.fPlayerTattoAlpha = 1.f;// 타투 알파값
	m_tPlayerEyeShaderDesc.fPlayerTattoScale = 0.5;  //타투 크기
	m_tPlayerEyeShaderDesc.fPlayerTattoOffsetU = 0.5f; //타투 X위치
	m_tPlayerEyeShaderDesc.fPlayerTattoOffsetV = 0.5f; //타투 y위치
	m_tPlayerEyeShaderDesc.vPlayerTattoColor = { 1.f, 1.f, 1.f, 1.f };

	m_tHeadTextureChange.iEyeDetailNum = 16;
	m_tHeadTextureChange.iEyeHighlightNum = 8;
	m_tHeadTextureChange.iTattoPaintNum = 20;

	return S_OK;
}

HRESULT Player_Head::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_matCombinedMatrix), E_FAIL);

	if (FAILED(m_pShaderCom->Begin(1, true)))
		return E_FAIL;

	Model* pMasterRigModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		CHECK_FAILED(pMasterRigModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		CHECK_FAILED(m_pShaderCom->Bind_Resources(1), E_FAIL);

		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}
HRESULT Player_Head::Ready_BrowShaderValue()
{
	m_tPlayerBrowShaderDesc.vPlayerBrowColor = { 0.580f, 0.467f, 0.325f, 1.0f }; //눈썹색
	m_tPlayerBrowShaderDesc.vPlayerEyeLineColor = { 0.580f, 0.467f, 0.325f, 1.0f }; //아이라인칼라?
	m_tPlayerBrowShaderDesc.vPlayerLipColor = { 0.827f, 0.537f, 0.631f, 1.0f }; //립색깔
	m_tPlayerBrowShaderDesc.iPlayerUseSecondUV = 1; //두번째 UV사용
	m_tPlayerBrowShaderDesc.fPlayerBrowUVScale = { 1.8f,1.5f }; //UV스케일
	m_tPlayerBrowShaderDesc.vPlayerBrowUVOffset = { 0.f,0.23f }; //UV 위치?
	m_tPlayerBrowShaderDesc.fPlayerBrowUVRotation = XMConvertToRadians(55.f); //UV회전
	m_tPlayerBrowShaderDesc.fPlayerBrowXGap = { -0.20f }; //X갭?
	m_tPlayerBrowShaderDesc.fEyeLineAlpha = 0.3f; //아이라인알파
	m_tPlayerBrowShaderDesc.fLipAlpha = 0.4f; //립알파

	return S_OK;
}
HRESULT Player_Head::Ready_Components()
{
	CHECK_FAILED(Add_Component(_uint(LEVEL::STATIC), L"Prototype_Component_Shader_VTXPlayerAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerHead_Base"), Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceBrow"), Com_NewTexture, RCAST(Component**)(&m_pBrowTextureCom)), E_FAIL);
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceEyelash"), Com_NewTexture0, RCAST(Component**)(&m_pEyelashTexture)), E_FAIL);
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceEyewhite"), Com_NewTexture1, RCAST(Component**)(&m_pEyeWhiteTexture)), E_FAIL);
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceEyeDetail"), Com_NewTexture2, RCAST(Component**)(&m_pEyeDetailTexture)), E_FAIL);
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceEyeHighlight"), Com_NewTexture3, RCAST(Component**)(&m_pEyeHighlightTexture)), E_FAIL);
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFacePaints"), Com_NewTexture4, RCAST(Component**)(&m_pTattoTexture)), E_FAIL);

	Set_VecObjPassSize(m_pModelCom->Get_NumMeshes()); //사이즈 잡고 
	Set_AllPass_VecObjPass(0); //0으로 초기화 

	Set_Pass_VecObjPass(0, 6); //왼쪽 눈동자
	Set_Pass_VecObjPass(1, 6); //오른쪽 눈동자
	Set_Pass_VecObjPass(2, 5); //얼굴(피부/눈썹/아이라인/입술/눈물/혈관)
	Set_Pass_VecObjPass(3, 7); //치아 전용
	Set_Pass_VecObjPass(4, 7); //속눈썹

	// Dissolve용 텍스처
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture("Dissolve_Player"),
		L"Com_DissolveTexture", RCAST(Component**)(&m_pNoiseTextureCom)), E_FAIL);

	return S_OK;
}
HRESULT Player_Head::Bind_ShaderResources()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_matCombinedMatrix), E_FAIL);
	//CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);
	//CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}

Player_Head* Player_Head::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Player_Head* pInstance = new Player_Head(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Player_Head 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Player_Head::Clone(void* pArg)
{
	Player_Head* pInstance = new Player_Head(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Player_Head 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Player_Head::Free()
{
	__super::Free();
}