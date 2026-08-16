#include "Client_Define.h"
#include "Yakumo_Weapon.h"

#include "Shader.h"
#include "Model.h"

Client::Yakumo_Weapon::Yakumo_Weapon()
{
}

Client::Yakumo_Weapon::Yakumo_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice, pContext)
{
}

Client::Yakumo_Weapon::Yakumo_Weapon(const Yakumo_Weapon& original)
	:PartObject(original)
{
}

Client::Yakumo_Weapon::~Yakumo_Weapon()
{
}

HRESULT Client::Yakumo_Weapon::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Yakumo_Weapon::Initialize(void* arg)
{
	static _uint namenum = 0;

	YAKUMOWEAPON_DESC desc;
	YAKUMOWEAPON_DESC* pDesc = (arg == nullptr) ? &desc : static_cast<YAKUMOWEAPON_DESC*>(arg);
	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{
		m_pSocketMatrix = pDesc->pSocketMatrix;
	}

	(pDesc)->wstrName = L"Yakumo_Weapon_" + to_wstring(namenum++);
	m_pDissolveTime = CAST(YAKUMOWEAPON_DESC*)(arg)->pDissolveTime;

	CHECK_FAILED(__super::Initialize(pDesc), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	return S_OK;
}

_int Client::Yakumo_Weapon::Update_Priority(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;


	return 0;
}

_int Client::Yakumo_Weapon::Update_Parallel(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	// 소켓 매트릭스에서 부모 행렬 갱신 후 전체 갱신
	_matrix SocketMatrix = NormalizeMatrix(XMLoadFloat4x4(m_pSocketMatrix));

	_matrix parentMatrix = SocketMatrix * XMLoadFloat4x4(m_matParentMatrix);
	m_pTransformCom->Set_Matrix(parentMatrix);

	parentMatrix = PreMatrix_X * parentMatrix;

	m_pModelCom->Update_CombinedMatrices_Weapon(parentMatrix);

#ifdef _DEBUG
	for (_uint i = 0; i < m_pModelCom->Get_Bones().size(); ++i)
	{
		m_pModelCom->Render_DebugBones_NoCS(XMMatrixIdentity(), i);
	}
#endif // _DEBUG

	return 0;
}

_int Client::Yakumo_Weapon::Update(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	return 0;
}

_int Client::Yakumo_Weapon::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::SHADOW, this);

	return 0;
}

HRESULT Client::Yakumo_Weapon::Render(const _float fTimeDelta)
{

	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);
	_float NoRim = 0;
	m_pShaderCom->Bind_RawValue_ByHandle(g_fRimOn, &NoRim, sizeof(_float));

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		_uint bitFlag = 0;
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitFlag), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &bitFlag), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitFlag), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitFlag, sizeof(_uint));

		/* 쉐이더의 n번 패스를 사용해서 그리겠다고 선언 */
		CHECK_FAILED(m_pShaderCom->Begin(10), E_FAIL);

		/* n번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(10), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT Client::Yakumo_Weapon::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_matCombinedMatrix), E_FAIL);

	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pShaderCom->Begin(5, true)))
			return E_FAIL;

		CHECK_FAILED(m_pShaderCom->Bind_Resources(5), E_FAIL); //패스번호맞춰서? 

		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

void Client::Yakumo_Weapon::Playing_WeaponClash_Sound()
{
	m_pGameInstance->Play_Sound("469905387", 0.4f, false);
	//m_pGameInstance->PlayGroup("ACTION_RENKETSU_SWING_04_Play", 0.5f, false);
	m_pGameInstance->Play_Sound_RandomInGroup("Weapon_MonsterClash", 0.2f, false);
}

HRESULT Client::Yakumo_Weapon::Ready_Components()
{
	CHECK_FAILED(Add_Shader(Proto_Shader(L"VTXMesh")), E_FAIL);
	CHECK_FAILED(Add_Model(Proto_Model(L"YakumoWeapon_BlackGreatSword")), E_FAIL);

	// Dissolve용 텍스처
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture("Dissolve_Player"),
		L"Com_DissolveTexture", RCAST(Component**)(&m_pNoiseTextureCom)), E_FAIL);

	return S_OK;
}

HRESULT Client::Yakumo_Weapon::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

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

	return S_OK;
}


Yakumo_Weapon* Client::Yakumo_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Yakumo_Weapon* pInstance = new Yakumo_Weapon(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Yakumo_Weapon 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


GameObject* Client::Yakumo_Weapon::Clone(void* pArg)
{
	Yakumo_Weapon* pInstance = new Yakumo_Weapon(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Yakumo_Weapon 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::Yakumo_Weapon::Free()
{
	__super::Free();
}
