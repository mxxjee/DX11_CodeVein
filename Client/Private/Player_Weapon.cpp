#include "Client_Define.h"
#include "Player_Weapon.h"
#include "Player_MasterRig.h"

Client::Player_Weapon::Player_Weapon()
{
}

Client::Player_Weapon::Player_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice,pContext)
{
}

Client::Player_Weapon::Player_Weapon(const Player_Weapon& original)
	:PartObject(original)
{
}

Client::Player_Weapon::~Player_Weapon()
{
}

HRESULT Client::Player_Weapon::Ready_Components()
{
	// Dissolve용 텍스처
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture("Dissolve_Player"),
		L"Com_DissolveTexture", RCAST(Component**)(&m_pNoiseTextureCom)), E_FAIL);

	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"RimNoise"), Com_Texture, RCAST(Component**)(&m_pRimNoiseTexture)), E_FAIL);

	return S_OK;
}

HRESULT Client::Player_Weapon::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Player_Weapon::Initialize(void* arg)
{
	PLAYERWEAPON_DESC Desc = {};
	PLAYERWEAPON_DESC* pDesc = static_cast<PLAYERWEAPON_DESC*>(arg);

	if (arg == nullptr)
	{
		arg = &Desc;
	}
	else
	{
		m_iSocketIndex = pDesc->iSocketIndex;
		m_pSocketMatrix = pDesc->pSocketMatrix;
		m_eWeaponType = pDesc->eWeaponType;
		m_pDissolveTime = pDesc->pDissolveTime;
	}

	CHECK_FAILED(PartObject::Initialize(pDesc), E_FAIL);

	m_pFocus = static_cast<Player*>(m_pGameInstance->Get_Player())->Get_FocuseStatePtr();

	return S_OK;
}

_int Client::Player_Weapon::Update_Priority(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	return 0;
}

_int Client::Player_Weapon::Update_Parallel(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	// 소켓 매트릭스에서 부모 행렬 갱신 후 전체 갱신
	const _float4x4* socket = CAST(Player_MasterRig*)(m_pMasterRig)->Get_SocketMatrix(m_iSocketIndex);
	_matrix SocketMatrix = NormalizeMatrix(XMLoadFloat4x4(socket));

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

_int Client::Player_Weapon::Update(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;
	
	m_iRimNoiseTime += fTimeDelta;

	return 0;
}

_int Client::Player_Weapon::Update_Late(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	return 0;
}

HRESULT Client::Player_Weapon::Render(const _float fTimeDelta)
{
	// Noise Texture
	_uint bDissolving = (m_pDissolveTime && *m_pDissolveTime > 0.f);
	_uint bUseNoise = bDissolving ? 1 : 0;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseNoise, &bUseNoise, sizeof(_uint)), E_FAIL);
	if (bUseNoise)
	{
		_float fTime = *m_pDissolveTime * 2.f;
		CHECK_FAILED(m_pNoiseTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 12, 0), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_fTimeElapsed, &fTime, sizeof(_float));
	}

	//Rim NoiseTexture
	_uint bUseRimNoise = *m_pFocus ? 1 : 0;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_fRimOn, &bUseRimNoise, sizeof(_uint)), E_FAIL);
	if (*m_pFocus)
	{
		CHECK_FAILED(m_pRimNoiseTexture->Bind_ShaderResource_FullSlot(m_pShaderCom, 13, 0), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_fTime, &m_iRimNoiseTime, sizeof(_float));
	}
		
	return S_OK;
}

void Client::Player_Weapon::Free()
{
	__super::Free();
}
