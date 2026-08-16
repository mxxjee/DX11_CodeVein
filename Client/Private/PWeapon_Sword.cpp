#include "Client_Define.h"
#include "PWeapon_Sword.h"

#include "Shader.h"
#include "Model.h"

Client::PWeapon_Sword::PWeapon_Sword()
{
}

Client::PWeapon_Sword::PWeapon_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Player_Weapon(pDevice, pContext)
{
}

Client::PWeapon_Sword::PWeapon_Sword(const PWeapon_Sword& original)
	:Player_Weapon(original)
{
}

Client::PWeapon_Sword::~PWeapon_Sword()
{
}

HRESULT Client::PWeapon_Sword::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::PWeapon_Sword::Initialize(void* arg)
{
	static _uint namenum = 0;

	WEAPONSWORD_DESC desc;
	WEAPONSWORD_DESC* pDesc = (arg == nullptr) ? &desc : static_cast<WEAPONSWORD_DESC*>(arg);
	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{
		_uint i = RCAST(_uint)(pDesc->voidValue0);
	}

	(pDesc)->wstrName = L"Player_Weapon_Sword" + to_wstring(namenum++);

	CHECK_FAILED(__super::Initialize(pDesc), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	//m_pTransformCom->Rotation(XMConvertToRadians(-90.f), 0.f, 0.f);

	return S_OK;
}

_int Client::PWeapon_Sword::Update_Priority(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	return 0;
}

_int Client::PWeapon_Sword::Update_Parallel(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	__super::Update_Parallel(fTimeDelta);

	return 0;
}

_int Client::PWeapon_Sword::Update(const _float fTimeDelta)
{
	Player_Weapon::Update(fTimeDelta);

	if (!m_bActive)
		return -1;

	return 0;
}

_int Client::PWeapon_Sword::Update_Late(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::SHADOW, this);

	return 0;
}

HRESULT Client::PWeapon_Sword::Render(const _float fTimeDelta)
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

HRESULT Client::PWeapon_Sword::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);
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

_int Client::PWeapon_Sword::RequestAnimIndex(const ANIMREQUEST_DESC& tAnimRequest)
{
	switch (tAnimRequest.eWeaponAction)
	{
	case WEAPON_ANIM_ACTION::IDLE:
		return 0;
	case WEAPON_ANIM_ACTION::ATTACKNORMAL:
	{
		switch (tAnimRequest.iCombo)
		{
		case 0: return 137;
		case 1: return 138;
		case 2: return 139;
		case 3: return 140;
		default:return 137;
		}
	}
	case WEAPON_ANIM_ACTION::ATTACKSTRONG:
		return 141;
	case WEAPON_ANIM_ACTION::ATTACKSTRONGCHARGE:
		return 142;
	case WEAPON_ANIM_ACTION::SPECIALDOWN2:
		return 144;
	case WEAPON_ANIM_ACTION::SPECIALLAUNCH:
		return 143;
	case WEAPON_ANIM_ACTION::ATTACKDODGE_F:
		return 145;
	case WEAPON_ANIM_ACTION::ATTACKDODGE_B:
		return 146;
	case WEAPON_ANIM_ACTION::DODGEROLL:
		switch (tAnimRequest.eDir)
		{
		case INPUT_DIR::LEFT: return 153;
		case INPUT_DIR::RIGHT: return 154;
		case INPUT_DIR::UP: return 150;
		case INPUT_DIR::DOWN: return 147;
		case INPUT_DIR::LEFT_UP: return 151;
		case INPUT_DIR::LEFT_DOWN: return 148;
		case INPUT_DIR::RIGHT_UP: return 152;
		case INPUT_DIR::RIGHT_DOWN: return 149;
		default:return 150;
		}
	case WEAPON_ANIM_ACTION::DODGEROLL_BACK:
		return 147;
	case WEAPON_ANIM_ACTION::GUARD_START:return 157;
	case WEAPON_ANIM_ACTION::GUARD_LOOP:return 158;
	case WEAPON_ANIM_ACTION::GUARD_END:return 159;
	case WEAPON_ANIM_ACTION::GUARD_HIT:return 155;
	case WEAPON_ANIM_ACTION::GUARD_BREAK:return 156;
	default:
		return -1;
	}
}

void Client::PWeapon_Sword::Playing_WeaponClash_Sound()
{
	m_pGameInstance->Play_Sound("LONG_SWORD_HIT_03_Play (268436447)", 0.15f, false);
	m_pGameInstance->Play_Sound_RandomInGroup("Weapon_MonsterClash", 0.2f, false);
}

HRESULT Client::PWeapon_Sword::Ready_Components()
{
	CHECK_FAILED(Player_Weapon::Ready_Components(), E_FAIL);
	CHECK_FAILED(Add_Shader(Proto_Shader(L"VTXMesh")), E_FAIL);
	CHECK_FAILED(Add_Model(Proto_Model(L"PlayerWeapon_BlackSword")), E_FAIL);

	return S_OK;
}

HRESULT Client::PWeapon_Sword::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

	return S_OK;
}


PWeapon_Sword* Client::PWeapon_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	PWeapon_Sword* pInstance = new PWeapon_Sword(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"PWeapon_Sword 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


GameObject* Client::PWeapon_Sword::Clone(void* pArg)
{
	PWeapon_Sword* pInstance = new PWeapon_Sword(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"PWeapon_Sword 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::PWeapon_Sword::Free()
{
	__super::Free();
}
