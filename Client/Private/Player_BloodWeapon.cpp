#include "Client_Define.h"
#include "Player_BloodWeapon.h"
#include "Model.h"
#include "Shader.h"
#include "Bone.h"
#include "Player_MasterRig.h"

Player_BloodWeapon::Player_BloodWeapon()
{
}

Player_BloodWeapon::Player_BloodWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice, pContext)
{
}

Player_BloodWeapon::Player_BloodWeapon(const Player_BloodWeapon& original)
	:PartObject(original)
{
}

Player_BloodWeapon::~Player_BloodWeapon()
{
}
HRESULT Player_BloodWeapon::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;

}

HRESULT Player_BloodWeapon::Initialize(void* arg)
{
	static _uint namenum = 0;

	BLOODWEAPON_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(BLOODWEAPON_DESC*)(arg)->wstrName = L"Player_BloodWeapon" + to_wstring(namenum++);

	CHECK_FAILED(PartObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);
	CHECK_FAILED(Ready_Events(), E_FAIL);

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
		if (MyBones[i]->Get_ParentBoneIndex() == 2)
			cout << "[ROOT] " << i << " : " << MyBones[i]->Get_BoneName() << endl;
	}

	Model* pMasterModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
	pMasterModelCom->RegisterPartModelCom(m_pModelCom);

	m_pModelCom->Ready_ReampSRV(pMasterModelCom, m_BoneRemap);

	//////////////////////////////////////////////////////////////////////////
	m_bIsVisible = false;////////////////// 임시로 안 보이게 해놓음
	//////////////////////////////////////////////////////////////////////////
	
	Set_AllPass_VecObjPass(9);

	m_pFocus = static_cast<Player*>(m_pGameInstance->Get_Player())->Get_FocuseStatePtr();

	return S_OK;
}
_int Player_BloodWeapon::Update_Priority(const _float fTimeDelta)
{


	return 0;
}

_int Player_BloodWeapon::Update(const _float fTimeDelta)
{
	__super::Setup_CombinedWorldMatrix(XMLoadFloat4x4(m_matParentMatrix));
	m_iRimNoiseTime += fTimeDelta;

	if (m_bDissolving)
	{
		m_fTimeElapsed += fTimeDelta * m_fDissolveDir;
		m_fTimeElapsed = clamp(m_fTimeElapsed, 0.f, m_fDissolveMax);

		// 완료 체크
		if ((m_fDissolveDir > 0.f && m_fTimeElapsed >= m_fDissolveMax) ||
			(m_fDissolveDir < 0.f && m_fTimeElapsed <= 0.f))
			m_bDissolving = false;
	}

	return 0;
}

_int Player_BloodWeapon::Update_Late(const _float fTimeDelta)
{

	GameObject::Add_RenderGroup(RENDER_GROUP::NONBLEND); //
	GameObject::Add_RenderGroup(RENDER_GROUP::SHADOW); //

	return 0;
}


HRESULT Player_BloodWeapon::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	Model* pMasterRigModelCom = CAST(Model*)(m_pMasterRig->Get_Component_FromName(Com_Model));
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);

	// Noise Texture
	_uint bDissolving = (m_fTimeElapsed > 0.f);
	_uint bUseNoise = bDissolving ? 1 : 0;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseNoise, &bUseNoise, sizeof(_uint)), E_FAIL);
	if (bUseNoise)
	{
		CHECK_FAILED(m_pNoiseTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 12, 0), E_FAIL);
		
		_float fNormalized = m_fTimeElapsed / m_fDissolveMax;
		m_pShaderCom->Bind_RawValue_ByHandle(g_fTimeElapsed, &fNormalized, sizeof(_float));
	}

	//Rim NoiseTexture
	if (*m_pFocus)
	{
		CHECK_FAILED(m_pRimNoiseTexture->Bind_ShaderResource_FullSlot(m_pShaderCom, 18, 0), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_fTime, &m_iRimNoiseTime, sizeof(_float));
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		/* 쉐이더의 0번 패스를 사용해서 그리겠다고 선언 */
		CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);

		//바디도 메쉬별로 바인딩 분류 해줘야하긴함.
		_uint bitflag = 0;
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL); 
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR, 0, &bitflag), E_FAIL); 
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &bitflag), E_FAIL); 
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitflag), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &bitflag), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));

		CHECK_FAILED(pMasterRigModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		/* 0번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT Player_BloodWeapon::Ready_Components()
{
	CHECK_FAILED(Add_Component(_uint(LEVEL::STATIC), L"Prototype_Component_Shader_VTXPlayerAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerBloodWeapon_DrapeWolf"), Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"RimNoise"), Com_Texture, RCAST(Component**)(&m_pRimNoiseTexture)), E_FAIL);

	Set_VecObjPassSize(m_pModelCom->Get_NumMeshes()); //사이즈 잡고

	// Dissolve용 텍스처
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture("Dissolve"),
		L"Com_DissolveTexture", RCAST(Component**)(&m_pNoiseTextureCom)), E_FAIL);

	return S_OK;
}
HRESULT Player_BloodWeapon::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
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


HRESULT Player_BloodWeapon::Bind_ShaderResources()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_matCombinedMatrix), E_FAIL);

	return S_OK;
}

HRESULT Client::Player_BloodWeapon::Ready_Events()
{
	// Dissolve 이벤트 구독
	Subscribe_Event<DISSOLVE_EVENT>([this](const DISSOLVE_EVENT& e) {
		if (!e.bIsWeaponTarget)
			return;

		m_bDissolving = true;

		if (e.bWeapon_Dissolve)
		{
			// 사라지기
			m_fDissolveDir = 1.f;
			m_fTimeElapsed = 0.f;
		}
		else
		{
			// 나타나기
			m_fDissolveDir = -1.f;
			m_fTimeElapsed = m_fDissolveMax;
		}
		});

	return S_OK;
}

Player_BloodWeapon* Player_BloodWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Player_BloodWeapon* pInstance = new Player_BloodWeapon(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Player_BloodWeapon 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Player_BloodWeapon::Clone(void* pArg)
{
	Player_BloodWeapon* pInstance = new Player_BloodWeapon(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Player_BloodWeapon 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Player_BloodWeapon::Free()
{
	__super::Free();
}