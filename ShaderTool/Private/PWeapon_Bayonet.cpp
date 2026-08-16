#include "ShaderTool_Define.h"
#include "PWeapon_Bayonet.h"

#include "Shader.h"
#include "Model.h"

ShaderTool::PWeapon_Bayonet::PWeapon_Bayonet()
{
}

ShaderTool::PWeapon_Bayonet::PWeapon_Bayonet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Player_Weapon(pDevice, pContext)
{
}

ShaderTool::PWeapon_Bayonet::PWeapon_Bayonet(const PWeapon_Bayonet& original)
	:Player_Weapon(original)
{
}

ShaderTool::PWeapon_Bayonet::~PWeapon_Bayonet()
{
}

HRESULT ShaderTool::PWeapon_Bayonet::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT ShaderTool::PWeapon_Bayonet::Initialize(void* arg)
{
	static _uint namenum = 0;

	WEAPONBAYONET_DESC desc;
	WEAPONBAYONET_DESC* pDesc = (arg == nullptr) ? &desc : static_cast<WEAPONBAYONET_DESC*>(arg);
	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	(pDesc)->wstrName = L"Player_Weapon_Bayonet" + to_wstring(namenum++);

	CHECK_FAILED(__super::Initialize(pDesc), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	// m_pTransformCom->Rotation(XMConvertToRadians(-90.f), 0.f, 0.f);

	// Trail 오프셋 조정
	m_vTrailRootOffset = { 0.f, 0.f, 0.85f };
	m_vTrailTipOffset = { 0.f, 0.f, 1.55f };

	return S_OK;
}

_int ShaderTool::PWeapon_Bayonet::Update_Priority(const _float fTimeDelta)
{

	return 0;
}

_int ShaderTool::PWeapon_Bayonet::Update_Parallel(const _float fTimeDelta)
{
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	_matrix parentMatrix = XMMatrixRotationX(XMConvertToRadians(270.f)) * SocketMatrix * XMLoadFloat4x4(m_matParentMatrix);

	m_pModelCom->Update_CombinedMatrices_Weapon(parentMatrix);

#ifdef _DEBUG
	for (_uint i = 0; i < m_pModelCom->Get_Bones().size(); ++i)
	{
		m_pModelCom->Render_DebugBones_NoCS(XMMatrixIdentity(), i);
	}
#endif // _DEBUG

	return 0;
}

_int ShaderTool::PWeapon_Bayonet::Update(const _float fTimeDelta)
{
	if (!m_bActive)
		return 0;

	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (int i = 0; i < 3; ++i)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	_matrix ParentMatrix = SocketMatrix * XMLoadFloat4x4(m_matParentMatrix);

	__super::Setup_CombinedWorldMatrix(ParentMatrix);

	return 0;
}

_int ShaderTool::PWeapon_Bayonet::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::SHADOW, this);

	return 0;
}

HRESULT ShaderTool::PWeapon_Bayonet::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		_uint test = 0;
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &test), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &test), E_FAIL);
		m_pShaderCom->Bind_RawValue_FullSlot(1, "g_bitFlag", &test, sizeof(_uint));

		/* 쉐이더의 n번 패스를 사용해서 그리겠다고 선언 */
		CHECK_FAILED(m_pShaderCom->Begin(7), E_FAIL);

		/* n번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(7), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT ShaderTool::PWeapon_Bayonet::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_OBJECT, "g_WorldMatrix", m_matCombinedMatrix), E_FAIL);
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

_int ShaderTool::PWeapon_Bayonet::RequestAnimIndex(const ANIMREQUEST_DESC& tAnimRequest)
{
	switch (tAnimRequest.eWeaponAction)
	{
	case WEAPON_ANIM_ACTION::IDLE:
		return 42;
	case WEAPON_ANIM_ACTION::ATTACKNORMAL:
	{
		switch (tAnimRequest.iCombo)
		{
		case 0: return 47;
		case 1: return 63;
		case 2: return 49;
		default:return -1;
		}
	}
	case WEAPON_ANIM_ACTION::ATTACKSTRONG:
		return 50;
	case WEAPON_ANIM_ACTION::SPECIALDOWN2:
		return 51;
	case WEAPON_ANIM_ACTION::SPECIALLAUNCH:
		return 52;
	case WEAPON_ANIM_ACTION::ATTACKDODGE_F:
		return 53;
	case WEAPON_ANIM_ACTION::ATTACKDODGE_B:
		return 54;
	case WEAPON_ANIM_ACTION::DODGEROLL:
		//switch (tAnimRequest.eDir)
		//{
		//	case INPUT_DIR::LEFT: return 56;
		//	case INPUT_DIR::RIGHT: return 55;
		//	case INPUT_DIR::UP: return 59;
		//	case INPUT_DIR::DOWN: return 59;
		//	case INPUT_DIR::LEFT_UP: return 58;
		//	case INPUT_DIR::LEFT_DOWN: return 61;
		//	case INPUT_DIR::RIGHT_UP: return 57;
		//	case INPUT_DIR::RIGHT_DOWN: return 60;
		//	default:return 59;
		//}
		return 59;
	case WEAPON_ANIM_ACTION::DODGEROLL_BACK:
		return 62;
	case WEAPON_ANIM_ACTION::GUARD_START:return 64;
	case WEAPON_ANIM_ACTION::GUARD_LOOP:return 65;
	case WEAPON_ANIM_ACTION::GUARD_END:return 66;
	case WEAPON_ANIM_ACTION::GUARD_HIT:return 68;
	default:
		return -1;
	}
}

HRESULT ShaderTool::PWeapon_Bayonet::Ready_Components()
{
	CHECK_FAILED(Add_Shader(Proto_Shader(L"VTXMesh")), E_FAIL);
	CHECK_FAILED(Add_Model(Proto_Model(L"PlayerWeapon_BlackBayonet")), E_FAIL);

	return S_OK;
}

HRESULT ShaderTool::PWeapon_Bayonet::Bind_ShaderResources()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_matCombinedMatrix), E_FAIL);
	CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}


PWeapon_Bayonet* ShaderTool::PWeapon_Bayonet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	PWeapon_Bayonet* pInstance = new PWeapon_Bayonet(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"PWeapon_Bayonet 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


GameObject* ShaderTool::PWeapon_Bayonet::Clone(void* pArg)
{
	PWeapon_Bayonet* pInstance = new PWeapon_Bayonet(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"PWeapon_Bayonet 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void ShaderTool::PWeapon_Bayonet::Free()
{
	__super::Free();
}
