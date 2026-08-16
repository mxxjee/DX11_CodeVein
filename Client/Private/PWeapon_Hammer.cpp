#include "Client_Define.h"
#include "PWeapon_Hammer.h"

#include "Shader.h"
#include "Model.h"

Client::PWeapon_Hammer::PWeapon_Hammer()
{
}

Client::PWeapon_Hammer::PWeapon_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Player_Weapon(pDevice, pContext)
{
}

Client::PWeapon_Hammer::PWeapon_Hammer(const PWeapon_Hammer& original)
	:Player_Weapon(original)
{
}

Client::PWeapon_Hammer::~PWeapon_Hammer()
{
}

HRESULT Client::PWeapon_Hammer::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::PWeapon_Hammer::Initialize(void* arg)
{
	static _uint namenum = 0;

	WEAPONHAMMER_DESC desc;
	WEAPONHAMMER_DESC* pDesc = (arg == nullptr) ? &desc : static_cast<WEAPONHAMMER_DESC*>(arg);
	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	(pDesc)->wstrName = L"Player_Weapon_Hammer" + to_wstring(namenum++);

	CHECK_FAILED(__super::Initialize(pDesc), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	//m_pTransformCom->Rotation(XMConvertToRadians(-90.f), 0.f, 0.f);

	return S_OK;
}

_int Client::PWeapon_Hammer::Update_Priority(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	return 0;
}

_int Client::PWeapon_Hammer::Update_Parallel(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	__super::Update_Parallel(fTimeDelta);

	return 0;
}

_int Client::PWeapon_Hammer::Update(const _float fTimeDelta)
{
	Player_Weapon::Update(fTimeDelta);

	if (!m_bActive)
		return -1;

	return 0;
}

_int Client::PWeapon_Hammer::Update_Late(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::SHADOW, this);

	return 0;
}

HRESULT Client::PWeapon_Hammer::Render(const _float fTimeDelta)
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

HRESULT Client::PWeapon_Hammer::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
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
_int Client::PWeapon_Hammer::RequestAnimIndex(const ANIMREQUEST_DESC& tAnimRequest)
{
	switch (tAnimRequest.eWeaponAction)
	{
	case WEAPON_ANIM_ACTION::IDLE:
		return 205;
	case WEAPON_ANIM_ACTION::ATTACKNORMAL:
	{
		switch (tAnimRequest.iCombo)
		{
		case 0: return 206;
		case 1: return 207;
		case 2: return 208;
		default:return 206;
		}
	}
	case WEAPON_ANIM_ACTION::ATTACKSTRONG:
		return 209;
	case WEAPON_ANIM_ACTION::ATTACKSTRONGCHARGE:
		return 210;
	case WEAPON_ANIM_ACTION::SPECIALDOWN2:
		return 211;
	case WEAPON_ANIM_ACTION::SPECIALLAUNCH:
		return 212;
	case WEAPON_ANIM_ACTION::ATTACKDODGE_F:
		return 213;
	case WEAPON_ANIM_ACTION::ATTACKDODGE_B:
		return 214;
	case WEAPON_ANIM_ACTION::DODGEROLL:
		switch (tAnimRequest.eDir)
		{
		case INPUT_DIR::LEFT: return 216;
		case INPUT_DIR::RIGHT: return 215;
		case INPUT_DIR::UP: return  219;
		case INPUT_DIR::DOWN: return 222;
		case INPUT_DIR::LEFT_UP: return 218;
		case INPUT_DIR::LEFT_DOWN: return 221;
		case INPUT_DIR::RIGHT_UP: return 217;
		case INPUT_DIR::RIGHT_DOWN: return 220;
		default:return 219;
		}
	case WEAPON_ANIM_ACTION::DODGEROLL_BACK:
		return 222;
	case WEAPON_ANIM_ACTION::GUARD_START:return 225;
	case WEAPON_ANIM_ACTION::GUARD_LOOP:return 226;
	case WEAPON_ANIM_ACTION::GUARD_END:return 227;
	case WEAPON_ANIM_ACTION::GUARD_HIT:return 223;
	case WEAPON_ANIM_ACTION::GUARD_BREAK:return 224;
	default:
		return -1;
	}
}

void Client::PWeapon_Hammer::Playing_WeaponClash_Sound()
{
	//m_pGameInstance->Play_Sound("469905387", 0.4f, false);
	//m_pGameInstance->PlayGroup("Hammer_Clash", 0.4f, false);
	
	m_pGameInstance->PlayGroup("Hammer_Hit", 0.2f, false);
	m_pGameInstance->Play_Sound_RandomInGroup("Weapon_MonsterClash", 0.2f, false);
}

HRESULT Client::PWeapon_Hammer::Ready_Components()
{
	CHECK_FAILED(Player_Weapon::Ready_Components(), E_FAIL);
	CHECK_FAILED(Add_Shader(Proto_Shader(L"VTXMesh")), E_FAIL);
	CHECK_FAILED(Add_Model(Proto_Model(L"PlayerWeapon_WhiteHammer")), E_FAIL);

	return S_OK;
}

HRESULT Client::PWeapon_Hammer::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

	return S_OK;
}


PWeapon_Hammer* Client::PWeapon_Hammer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	PWeapon_Hammer* pInstance = new PWeapon_Hammer(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"PWeapon_Hammer 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


GameObject* Client::PWeapon_Hammer::Clone(void* pArg)
{
	PWeapon_Hammer* pInstance = new PWeapon_Hammer(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"PWeapon_Hammer 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::PWeapon_Hammer::Free()
{
	__super::Free();
}
