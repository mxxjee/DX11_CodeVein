#include "Client_Define.h"
#include "PWeapon_Halberd.h"

#include "Shader.h"
#include "Model.h"

Client::PWeapon_Halberd::PWeapon_Halberd()
{
}

Client::PWeapon_Halberd::PWeapon_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Player_Weapon(pDevice, pContext)
{
}

Client::PWeapon_Halberd::PWeapon_Halberd(const PWeapon_Halberd& original)
	:Player_Weapon(original)
{
}

Client::PWeapon_Halberd::~PWeapon_Halberd()
{
}

HRESULT Client::PWeapon_Halberd::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::PWeapon_Halberd::Initialize(void* arg)
{
	static _uint namenum = 0;

	WEAPONHALBERD_DESC desc;
	WEAPONHALBERD_DESC* pDesc = (arg == nullptr) ? &desc : static_cast<WEAPONHALBERD_DESC*>(arg);
	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	(pDesc)->wstrName = L"Player_Weapon_Halberd" + to_wstring(namenum++);

	CHECK_FAILED(__super::Initialize(pDesc), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	//m_pTransformCom->Rotation(XMConvertToRadians(-90.f), 0.f, 0.f);

	return S_OK;
}

_int Client::PWeapon_Halberd::Update_Priority(const _float fTimeDelta)
{
	if (!m_bActive)
		return 0;

	return 0;
}

_int Client::PWeapon_Halberd::Update_Parallel(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	__super::Update_Parallel(fTimeDelta);

	return 0;
}

_int Client::PWeapon_Halberd::Update(const _float fTimeDelta)
{
	Player_Weapon::Update(fTimeDelta);

	if (!m_bActive)
		return -1;


	return 0;
}

_int Client::PWeapon_Halberd::Update_Late(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::SHADOW, this);

	return 0;
}

HRESULT Client::PWeapon_Halberd::Render(const _float fTimeDelta)
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

HRESULT Client::PWeapon_Halberd::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
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

_int Client::PWeapon_Halberd::RequestAnimIndex(const ANIMREQUEST_DESC& tAnimRequest)
{
	switch (tAnimRequest.eWeaponAction)
	{
	case WEAPON_ANIM_ACTION::IDLE:
		return 160;
	case WEAPON_ANIM_ACTION::ATTACKNORMAL:
	{
		switch (tAnimRequest.iCombo)
		{
		case 0: return 161;
		case 1: return 162;
		case 2: return 163;
		default:return 161;
		}
	}
	case WEAPON_ANIM_ACTION::ATTACKSTRONG:
		return 164;
	case WEAPON_ANIM_ACTION::ATTACKSTRONGCHARGE:
		return 165;
	case WEAPON_ANIM_ACTION::SPECIALDOWN2:
		return 166;
	case WEAPON_ANIM_ACTION::SPECIALLAUNCH:
		return 228;
	case WEAPON_ANIM_ACTION::ATTACKDODGE_F:
		return 167;
	case WEAPON_ANIM_ACTION::ATTACKDODGE_B:
		return 168;
	case WEAPON_ANIM_ACTION::DODGEROLL:
		switch (tAnimRequest.eDir)
		{
		case INPUT_DIR::LEFT: return 175;
		case INPUT_DIR::RIGHT: return 176;
		case INPUT_DIR::UP: return 172;
		case INPUT_DIR::DOWN: return 169;
		case INPUT_DIR::LEFT_UP: return 173;
		case INPUT_DIR::LEFT_DOWN: return 170;
		case INPUT_DIR::RIGHT_UP: return 174;
		case INPUT_DIR::RIGHT_DOWN: return 171;
		default:return 172;
		}
	case WEAPON_ANIM_ACTION::DODGEROLL_BACK:
		return 169;
	case WEAPON_ANIM_ACTION::GUARD_START:return 179;
	case WEAPON_ANIM_ACTION::GUARD_LOOP:return 180;
	case WEAPON_ANIM_ACTION::GUARD_END:return 181;
	case WEAPON_ANIM_ACTION::GUARD_HIT:return 177;
	case WEAPON_ANIM_ACTION::GUARD_BREAK:return 178;
	default:
		return -1;
	}
}

void Client::PWeapon_Halberd::Playing_WeaponClash_Sound()
{
	m_pGameInstance->Play_Sound("469905387", 0.15f, false);
	m_pGameInstance->PlayGroup("Hallerd_Slash", 0.2f, false);
	m_pGameInstance->Play_Sound_RandomInGroup("Weapon_MonsterClash", 0.2f, false);

}

HRESULT Client::PWeapon_Halberd::Ready_Components()
{
	CHECK_FAILED(Player_Weapon::Ready_Components(), E_FAIL);
	CHECK_FAILED(Add_Shader(Proto_Shader(L"VTXMesh")), E_FAIL);
	CHECK_FAILED(Add_Model(Proto_Model(L"PlayerWeapon_BlackHalberd")), E_FAIL);

	return S_OK;
}

HRESULT Client::PWeapon_Halberd::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

	return S_OK;
}


PWeapon_Halberd* Client::PWeapon_Halberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	PWeapon_Halberd* pInstance = new PWeapon_Halberd(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"PWeapon_Halberd 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


GameObject* Client::PWeapon_Halberd::Clone(void* pArg)
{
	PWeapon_Halberd* pInstance = new PWeapon_Halberd(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"PWeapon_Halberd 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::PWeapon_Halberd::Free()
{
	__super::Free();
}
