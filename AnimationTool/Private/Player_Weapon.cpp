#include "AnimationTool_Define.h"
#include "Player_Weapon.h"


AnimationTool::Player_Weapon::Player_Weapon()
{
}

AnimationTool::Player_Weapon::Player_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice,pContext)
{
}

AnimationTool::Player_Weapon::Player_Weapon(const Player_Weapon& original)
	:PartObject(original)
{
}

AnimationTool::Player_Weapon::~Player_Weapon()
{
}

HRESULT AnimationTool::Player_Weapon::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT AnimationTool::Player_Weapon::Initialize(void* arg)
{
	PLAYERWEAPON_DESC Desc = {};
	PLAYERWEAPON_DESC* pDesc = static_cast<PLAYERWEAPON_DESC*>(arg);

	if (arg == nullptr)
	{
		arg = &Desc;
	}
	else
	{

		m_pSocketMatrix = pDesc->pSocketMatrix;
		m_eWeaponType = pDesc->eWeaponType;
	}

	CHECK_FAILED(PartObject::Initialize(pDesc), E_FAIL);

	return S_OK;
}

_int AnimationTool::Player_Weapon::Update_Priority(const _float fTimeDelta)
{
	return 0;
}

_int AnimationTool::Player_Weapon::Update_Parallel(const _float fTimeDelta)
{
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	_matrix parentMatrix = XMMatrixRotationX(XMConvertToRadians(270.f)) * SocketMatrix * XMLoadFloat4x4(m_matParentMatrix);

	m_pModelCom->Update_CombinedMatrices_Weapon(parentMatrix);

	for (_uint i = 0; i < m_pModelCom->Get_Bones().size(); ++i)
	{
		m_pModelCom->Render_DebugBones_NoCS(XMMatrixIdentity(), i);
	}

	return 0;
}

_int AnimationTool::Player_Weapon::Update(const _float fTimeDelta)
{
	return 0;
}

_int AnimationTool::Player_Weapon::Update_Late(const _float fTimeDelta)
{
	return 0;
}

HRESULT AnimationTool::Player_Weapon::Render(const _float fTimeDelta)
{
	return S_OK;
}

void AnimationTool::Player_Weapon::Free()
{
	__super::Free();
}
