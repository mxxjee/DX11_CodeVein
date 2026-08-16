#include "Client_Define.h"
#include "Player_Injection.h"
#include "Model.h"
#include "Shader.h"
#include "Bone.h"
#include "Player_MasterRig.h"

Player_Injection::Player_Injection()
{
}

Player_Injection::Player_Injection(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice, pContext)
{
}

Player_Injection::Player_Injection(const Player_Injection& original)
	:PartObject(original)
{
}

Player_Injection::~Player_Injection()
{
}
HRESULT Player_Injection::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;

}

HRESULT Player_Injection::Initialize(void* arg)
{
	static _uint namenum = 0;

	INJECTION_DESC desc;
	INJECTION_DESC* pDesc = (arg == nullptr) ? &desc : static_cast<INJECTION_DESC*>(arg);
	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{
		m_iSocketIndex = pDesc->iSocketIndex;
		m_pSocketMatrix = pDesc->pSocketMatrix;
	}


	pDesc->wstrName = L"Player_Injection" + to_wstring(namenum++);

	CHECK_FAILED(PartObject::Initialize(pDesc), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	return S_OK;
}
_int Player_Injection::Update_Priority(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	return 0;
}

_int Client::Player_Injection::Update_Parallel(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	const _float4x4* socket = CAST(Player_MasterRig*)(m_pMasterRig)->Get_SocketMatrix(m_iSocketIndex);
	_matrix SocketMatrix = NormalizeMatrix(XMLoadFloat4x4(socket));

	_matrix parentMatrix = SocketMatrix * XMLoadFloat4x4(m_matParentMatrix);
	m_pTransformCom->Set_Matrix(parentMatrix);

	parentMatrix = PreMatrix_XY * parentMatrix; //이거 머지

	m_pModelCom->Update_CombinedMatrices_Weapon(parentMatrix);

#ifdef _DEBUG
	for (_uint i = 0; i < m_pModelCom->Get_Bones().size(); ++i)
	{
		m_pModelCom->Render_DebugBones_NoCS(XMMatrixIdentity(), i);
	}
#endif // _DEBUG

	return 0;
}

_int Player_Injection::Update(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	__super::Setup_CombinedWorldMatrix(XMLoadFloat4x4(m_matParentMatrix));

	COUT("x:" << m_pTransformCom->Get_Position_Float3().x);
	COUT("y:" << m_pTransformCom->Get_Position_Float3().y);
	COUT("z:" << m_pTransformCom->Get_Position_Float3().z);

	return 0;
}

_int Player_Injection::Update_Late(const _float fTimeDelta)
{
	if (!m_bActive)
		return -1;

	GameObject::Add_RenderGroup(RENDER_GROUP::NONBLEND); //
	GameObject::Add_RenderGroup(RENDER_GROUP::SHADOW); //

	return 0;
}


HRESULT Player_Injection::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		_uint bitflag = 0;
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &bitflag), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitflag), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));

		/* 쉐이더의 n번 패스를 사용해서 그리겠다고 선언 */
		CHECK_FAILED(m_pShaderCom->Begin(10), E_FAIL);

		/* n번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(10), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT Player_Injection::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(5, true)))
			return E_FAIL;

		CHECK_FAILED(m_pShaderCom->Bind_Resources(5), E_FAIL); //패스번호맞춰서? 

		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}

HRESULT Player_Injection::Ready_Components()
{
	CHECK_FAILED(Add_Shader(Proto_Shader(L"VTXMesh")), E_FAIL);
	CHECK_FAILED(Add_Model(Proto_Model(L"Player_Injection")), E_FAIL);

	return S_OK;
}


HRESULT Player_Injection::Bind_ShaderResources()
{
	
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

	return S_OK;
}

Player_Injection* Player_Injection::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Player_Injection* pInstance = new Player_Injection(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Player_Injection 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Player_Injection::Clone(void* pArg)
{
	Player_Injection* pInstance = new Player_Injection(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Player_Injection 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Player_Injection::Free()
{
	__super::Free();
}