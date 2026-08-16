#include "AnimationTool_Define.h"
#include "MWeapon_Hammer.h"

#include "Shader.h"
#include "Model.h"

MWeapon_Hammer::MWeapon_Hammer()
{
}

MWeapon_Hammer::MWeapon_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Monster_Weapon(pDevice, pContext)
{
}

MWeapon_Hammer::MWeapon_Hammer(const MWeapon_Hammer& original)
	:Monster_Weapon(original)
{
}

MWeapon_Hammer::~MWeapon_Hammer()
{
}

HRESULT MWeapon_Hammer::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT MWeapon_Hammer::Initialize(void* arg)
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

	(pDesc)->wstrName = L"Monster_Weapon_Hammer" + to_wstring(namenum++);

	CHECK_FAILED(__super::Initialize(pDesc), E_FAIL);

	CHECK_FAILED(Ready_Components(pDesc->wstrModelTag), E_FAIL);

	//m_pTransformCom->Rotation(XMConvertToRadians(-90.f), 0.f, 0.f);
	return S_OK;
}

_int MWeapon_Hammer::Update_Priority(const _float fTimeDelta)
{

	return 0;
}

_int MWeapon_Hammer::Update_Parallel(const _float fTimeDelta)
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

_int MWeapon_Hammer::Update(const _float fTimeDelta)
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

_int MWeapon_Hammer::Update_Late(const _float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);

	return 0;
}

HRESULT MWeapon_Hammer::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE), E_FAIL);

		CHECK_FAILED(m_pShaderCom->Begin(0), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_Resources(0), E_FAIL);

		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT MWeapon_Hammer::Ready_Components(const wstring& wstrModelTag)
{
	CHECK_FAILED(Add_Shader(Proto_Shader(L"VTXMesh")), E_FAIL);
	if (wstrModelTag.empty())
	{
		// 예외 처리: 외부에서 이름을 안 넘겼을 때의 기본값
		CHECK_FAILED(Add_Model(Proto_Model(L"PlayerWeapon_WhiteHammer")), E_FAIL);
	}
	else
	{
		CHECK_FAILED(Add_Model(wstrModelTag.c_str()), E_FAIL);
	}
	return S_OK;
}

HRESULT MWeapon_Hammer::Bind_ShaderResources()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_OBJECT, "g_WorldMatrix", m_matCombinedMatrix), E_FAIL);
	CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}


MWeapon_Hammer* MWeapon_Hammer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	MWeapon_Hammer* pInstance = new MWeapon_Hammer(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"MWeapon_Hammer 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


GameObject* MWeapon_Hammer::Clone(void* pArg)
{
	MWeapon_Hammer* pInstance = new MWeapon_Hammer(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"MWeapon_Hammer 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void MWeapon_Hammer::Free()
{
	__super::Free();
}
