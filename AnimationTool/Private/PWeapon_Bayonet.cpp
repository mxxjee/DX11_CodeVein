#include "AnimationTool_Define.h"
#include "PWeapon_Bayonet.h"

AnimationTool::PWeapon_Bayonet::PWeapon_Bayonet()
{
}

AnimationTool::PWeapon_Bayonet::PWeapon_Bayonet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Player_Weapon(pDevice,pContext)
{
}

AnimationTool::PWeapon_Bayonet::PWeapon_Bayonet(const PWeapon_Bayonet& original)
	:Player_Weapon(original)
{
}

AnimationTool::PWeapon_Bayonet::~PWeapon_Bayonet()
{
}

HRESULT AnimationTool::PWeapon_Bayonet::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);



	return S_OK;
}

HRESULT AnimationTool::PWeapon_Bayonet::Initialize(void* arg)
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

	m_pTransformCom->Rotation(XMConvertToRadians(0.f), XMConvertToRadians(-180.f), XMConvertToRadians(0.f));

	return S_OK;
}

_int AnimationTool::PWeapon_Bayonet::Update_Priority(const _float fTimeDelta)
{

	return 0;
}

_int AnimationTool::PWeapon_Bayonet::Update_Parallel(const _float fTimeDelta)
{
	Player_Weapon::Update_Parallel(fTimeDelta);

	return 0;
}

_int AnimationTool::PWeapon_Bayonet::Update(const _float fTimeDelta)
{
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (int i = 0; i < 3; ++i)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	_matrix ParentMatrix = SocketMatrix * XMLoadFloat4x4(m_matParentMatrix);

	__super::Setup_CombinedWorldMatrix(ParentMatrix);
	
	return 0;
}

_int AnimationTool::PWeapon_Bayonet::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);

	return 0;
}

HRESULT AnimationTool::PWeapon_Bayonet::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE), E_FAIL);

		/* 쉐이더의 n번 패스를 사용해서 그리겠다고 선언 */
		CHECK_FAILED(m_pShaderCom->Begin(0), E_FAIL);

		/* n번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(0), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT AnimationTool::PWeapon_Bayonet::Ready_Components()
{
	CHECK_FAILED(Add_Shader(Proto_Shader(L"VTXMesh")), E_FAIL);
	CHECK_FAILED(Add_Model(Proto_Model(L"PlayerWeapon_BlackBayonet")), E_FAIL);

	return S_OK;
}

HRESULT AnimationTool::PWeapon_Bayonet::Bind_ShaderResources()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_OBJECT, "g_WorldMatrix", m_matCombinedMatrix), E_FAIL);
	CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}


PWeapon_Bayonet* AnimationTool::PWeapon_Bayonet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	PWeapon_Bayonet* pInstance = new PWeapon_Bayonet(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"PWeapon_Bayonet 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


GameObject* AnimationTool::PWeapon_Bayonet::Clone(void* pArg)
{
	PWeapon_Bayonet* pInstance = new PWeapon_Bayonet(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"PWeapon_Bayonet 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void AnimationTool::PWeapon_Bayonet::Free()
{
	__super::Free();
}
