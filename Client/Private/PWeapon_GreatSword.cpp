#include "Client_Define.h"
#include "PWeapon_GreatSword.h"

#include "Shader.h"
#include "Model.h"

Client::PWeapon_GreatSword::PWeapon_GreatSword()
{
}

Client::PWeapon_GreatSword::PWeapon_GreatSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Player_Weapon(pDevice, pContext)
{
}

Client::PWeapon_GreatSword::PWeapon_GreatSword(const PWeapon_GreatSword& original)
	:Player_Weapon(original)
{
}

Client::PWeapon_GreatSword::~PWeapon_GreatSword()
{
}

HRESULT Client::PWeapon_GreatSword::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::PWeapon_GreatSword::Initialize(void* arg)
{
	static _uint namenum = 0;

	WEAPONGREATSWORD_DESC desc;
	WEAPONGREATSWORD_DESC* pDesc = (arg == nullptr) ? &desc : static_cast<WEAPONGREATSWORD_DESC*>(arg);
	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	(pDesc)->wstrName = L"Player_Weapon_GreatSword" + to_wstring(namenum++);

	CHECK_FAILED(__super::Initialize(pDesc), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	//m_pTransformCom->Rotation(XMConvertToRadians(-90.f), 0.f, 0.f);

	return S_OK;
}

_int Client::PWeapon_GreatSword::Update_Priority(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;


	return 0;
}

_int Client::PWeapon_GreatSword::Update_Parallel(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	__super::Update_Parallel(fTimeDelta);

	return 0;
}

_int Client::PWeapon_GreatSword::Update(const _float fTimeDelta)
{
	Player_Weapon::Update(fTimeDelta);

	if (!m_bActive)
		return -1;

	return 0;
}

_int Client::PWeapon_GreatSword::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::SHADOW, this);

	return 0;
}

HRESULT Client::PWeapon_GreatSword::Render(const _float fTimeDelta)
{
	// Dissolve 텍스처 바인딩용 호출
	CHECK_FAILED(Player_Weapon::Render(fTimeDelta), E_FAIL);

	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

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

HRESULT Client::PWeapon_GreatSword::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_matCombinedMatrix), E_FAIL);
	//CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);

	//CHECK_FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	//CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_LightMatrix(D3DTRANSFORM::D3DTS_VIEW)), E_FAIL);
	//CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_LightMatrix(D3DTRANSFORM::D3DTS_PROJ)), E_FAIL);
	//cout << m_iObjectID << endl;
	//CHECK_FAILED(Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/*if (FAILED(m_pModelCom->Bind_Bones(m_pShaderCom, "g_BonesMatrices", i)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(5, true)))
			return E_FAIL;

		CHECK_FAILED(m_pShaderCom->Bind_Resources(5), E_FAIL); //패스번호맞춰서? 

		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}

_int Client::PWeapon_GreatSword::RequestAnimIndex(const ANIMREQUEST_DESC& tAnimRequest)
{
	switch (tAnimRequest.eWeaponAction)
	{
	case WEAPON_ANIM_ACTION::IDLE:
		return 182;
	case WEAPON_ANIM_ACTION::ATTACKNORMAL:
	{
		switch (tAnimRequest.iCombo)
		{
		case 0: return 183;
		case 1: return 184;
		case 2: return 185;
		default:return 183;
		}
	}
	case WEAPON_ANIM_ACTION::ATTACKSTRONG:
		return 187;
	case WEAPON_ANIM_ACTION::ATTACKSTRONGCHARGE:
		return 186;
	case WEAPON_ANIM_ACTION::SPECIALDOWN2:
		return 188;
	case WEAPON_ANIM_ACTION::SPECIALLAUNCH:
		return 189;
	case WEAPON_ANIM_ACTION::ATTACKDODGE_F:
		return 190;
	case WEAPON_ANIM_ACTION::ATTACKDODGE_B:
		return 191;
	case WEAPON_ANIM_ACTION::DODGEROLL:
		switch (tAnimRequest.eDir)
		{
		case INPUT_DIR::LEFT: return 198;
		case INPUT_DIR::RIGHT: return 199;
		case INPUT_DIR::UP: return 195;
		case INPUT_DIR::DOWN: return 192;
		case INPUT_DIR::LEFT_UP: return 196;
		case INPUT_DIR::LEFT_DOWN: return 193;
		case INPUT_DIR::RIGHT_UP: return 197;
		case INPUT_DIR::RIGHT_DOWN: return 194;
		default:return 195;
		}
	case WEAPON_ANIM_ACTION::DODGEROLL_BACK:
		return 192;
	case WEAPON_ANIM_ACTION::GUARD_START:return 202;
	case WEAPON_ANIM_ACTION::GUARD_LOOP:return 203;
	case WEAPON_ANIM_ACTION::GUARD_END:return 204;
	case WEAPON_ANIM_ACTION::GUARD_HIT:return 200;
	case WEAPON_ANIM_ACTION::GUARD_BREAK:return 201;
	default:
		return -1;
	}
}

void Client::PWeapon_GreatSword::Playing_WeaponClash_Sound()
{
	m_pGameInstance->Play_Sound("469905387", 0.4f, false);
	//m_pGameInstance->PlayGroup("ACTION_RENKETSU_SWING_04_Play", 0.5f, false);
	m_pGameInstance->Play_Sound_RandomInGroup("Weapon_MonsterClash", 0.2f, false);
}

HRESULT Client::PWeapon_GreatSword::Ready_Components()
{
	CHECK_FAILED(Player_Weapon::Ready_Components(), E_FAIL);
	CHECK_FAILED(Add_Shader(Proto_Shader(L"VTXMesh")), E_FAIL);
	CHECK_FAILED(Add_Model(Proto_Model(L"PlayerWeapon_BlackGreatSword")), E_FAIL);

	return S_OK;
}

HRESULT Client::PWeapon_GreatSword::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

	return S_OK;
}


PWeapon_GreatSword* Client::PWeapon_GreatSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	PWeapon_GreatSword* pInstance = new PWeapon_GreatSword(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"PWeapon_GreatSword 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


GameObject* Client::PWeapon_GreatSword::Clone(void* pArg)
{
	PWeapon_GreatSword* pInstance = new PWeapon_GreatSword(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"PWeapon_GreatSword 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::PWeapon_GreatSword::Free()
{
	__super::Free();
}
