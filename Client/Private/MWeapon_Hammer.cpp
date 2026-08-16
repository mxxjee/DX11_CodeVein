#include "Client_Define.h"
#include "MWeapon_Hammer.h"

#include "Shader.h"
#include "Model.h"

Client::MWeapon_Hammer::MWeapon_Hammer()
{
}

Client::MWeapon_Hammer::MWeapon_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Monster_Weapon(pDevice, pContext)
{
}

Client::MWeapon_Hammer::MWeapon_Hammer(const MWeapon_Hammer& original)
	:Monster_Weapon(original)
{
}

Client::MWeapon_Hammer::~MWeapon_Hammer()
{
}

HRESULT Client::MWeapon_Hammer::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::MWeapon_Hammer::Initialize(void* arg)
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

	CHECK_FAILED(Ready_Components(), E_FAIL);

	Set_AllPass_VecObjPass(10);

	return S_OK;
}

_int Client::MWeapon_Hammer::Update_Priority(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Priority(fTimeDelta));

	return 0;
}

_int Client::MWeapon_Hammer::Update_Parallel(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Parallel(fTimeDelta));

	return 0;
}

_int Client::MWeapon_Hammer::Update(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update(fTimeDelta));

	//_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	//
	//for (int i = 0; i < 3; ++i)
	//{
	//	SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	//}
	//
	//_matrix ParentMatrix = SocketMatrix * XMLoadFloat4x4(m_matParentMatrix);

	//__super::Setup_CombinedWorldMatrix(ParentMatrix);

	return 0;
}

_int Client::MWeapon_Hammer::Update_Late(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Late(fTimeDelta));

	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::SHADOW, this);

	return 0;
}

HRESULT Client::MWeapon_Hammer::Render(const _float fTimeDelta)
{
	if (!m_bIsVisible || !m_bIsActive)
		return S_OK;

	Monster_Weapon::Render(fTimeDelta);

	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		_uint bitflag = 0;
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &bitflag), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &bitflag), E_FAIL);

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

HRESULT Client::MWeapon_Hammer::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	// Noise Texture
	_uint bDissolving = (m_pDissolveTime && *m_pDissolveTime > 0.f);
	_uint bUseNoise = bDissolving ? 1 : 0;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseNoise, &bUseNoise, sizeof(_uint)), E_FAIL);
	if (bUseNoise)
	{
		_float fTime = *m_pDissolveTime;
		_float fMax = (m_pDissolveMax && *m_pDissolveMax > 0.f) ? *m_pDissolveMax : 1.f;
		_float fNormalized = fTime / fMax;

		CHECK_FAILED(m_pNoiseTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 12, 0), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_fTimeElapsed, &fNormalized, sizeof(_float));
	}

	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		/*if (FAILED(m_pModelCom->Bind_Bones(m_pShaderCom, "g_BonesMatrices", i)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(5, false)))
			return E_FAIL;

		CHECK_FAILED(m_pShaderCom->Bind_Resources(5), E_FAIL); //패스번호맞춰서? 

		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}

HRESULT Client::MWeapon_Hammer::Ready_Components()
{
	CHECK_FAILED(Add_Shader(Proto_Shader(L"VTXMesh")), E_FAIL);
	CHECK_FAILED(Add_Model(L"Prototype_Component_Model_BossWeapon_OliverP01"), E_FAIL);

	// Dissolve용 텍스처 컴포넌트 추가
	CHECK_FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), Proto_Texture("Dissolve"),
		L"Com_DissolveTexture", RCAST(Component**)(&m_pNoiseTextureCom)), E_FAIL);

	Set_VecObjPassSize(m_pModelCom->Get_NumMeshes());

	return S_OK;
}

HRESULT Client::MWeapon_Hammer::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);
	//CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}


MWeapon_Hammer* Client::MWeapon_Hammer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	MWeapon_Hammer* pInstance = new MWeapon_Hammer(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"MWeapon_Hammer 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


GameObject* Client::MWeapon_Hammer::Clone(void* pArg)
{
	MWeapon_Hammer* pInstance = new MWeapon_Hammer(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"MWeapon_Hammer 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::MWeapon_Hammer::Free()
{
	__super::Free();
}
