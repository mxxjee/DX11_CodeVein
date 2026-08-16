#include "AnimationTool_Define.h"
#include "Yakumo_Weapon.h"

#include "Shader.h"
#include "Model.h"

AnimationTool::Yakumo_Weapon::Yakumo_Weapon()
{
}

AnimationTool::Yakumo_Weapon::Yakumo_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice, pContext)
{
}

AnimationTool::Yakumo_Weapon::Yakumo_Weapon(const Yakumo_Weapon& original)
	:PartObject(original)
{
}

AnimationTool::Yakumo_Weapon::~Yakumo_Weapon()
{
}

HRESULT AnimationTool::Yakumo_Weapon::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT AnimationTool::Yakumo_Weapon::Initialize(void* arg)
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

	CHECK_FAILED(__super::Initialize(pDesc), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	return S_OK;
}

_int AnimationTool::Yakumo_Weapon::Update_Priority(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;


	return 0;
}

_int AnimationTool::Yakumo_Weapon::Update_Parallel(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	// 소켓 매트릭스에서 부모 행렬 갱신 후 전체 갱신
	_matrix SocketMatrix = NormalizeMatrix(XMLoadFloat4x4(m_pSocketMatrix));

	_matrix parentMatrix = SocketMatrix * XMLoadFloat4x4(m_matParentMatrix);
	m_pTransformCom->Set_Matrix(parentMatrix);

	_matrix PreMatrix_X = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationX(XMConvertToRadians(-90.f));

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

_int AnimationTool::Yakumo_Weapon::Update(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	return 0;
}

_int AnimationTool::Yakumo_Weapon::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);

	return 0;
}

HRESULT AnimationTool::Yakumo_Weapon::Render(const _float fTimeDelta)
{

	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	m_pShaderCom->Begin(0);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE), E_FAIL);

		//CHECK_FAILED(pMasterRigModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ShadCTexture", i, aiTextureType_EMISSIVE), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);


		//CHECK_FAILED(m_pModelCom->Bind_Morphs(m_pShaderCom, i), E_FAIL);

		/* 0번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Commit(0), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}

HRESULT AnimationTool::Yakumo_Weapon::Ready_Components()
{
	CHECK_FAILED(Add_Shader(Proto_Shader(L"VTXMesh")), E_FAIL);
	CHECK_FAILED(Add_Model(Proto_Model(L"YakumoWeapon_BlackGreatSword")), E_FAIL);

	return S_OK;
}

HRESULT AnimationTool::Yakumo_Weapon::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

	return S_OK;
}


Yakumo_Weapon* AnimationTool::Yakumo_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Yakumo_Weapon* pInstance = new Yakumo_Weapon(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Yakumo_Weapon 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


GameObject* AnimationTool::Yakumo_Weapon::Clone(void* pArg)
{
	Yakumo_Weapon* pInstance = new Yakumo_Weapon(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Yakumo_Weapon 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void AnimationTool::Yakumo_Weapon::Free()
{
	__super::Free();
}
