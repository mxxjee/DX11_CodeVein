#include "Client_Define.h"
#include "MWeapon_Halberd.h"

#include "Shader.h"
#include "Model.h"

Client::MWeapon_Halberd::MWeapon_Halberd()
{
}

Client::MWeapon_Halberd::MWeapon_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Monster_Weapon(pDevice, pContext)
{
}

Client::MWeapon_Halberd::MWeapon_Halberd(const MWeapon_Halberd& original)
	:Monster_Weapon(original)
{
}

Client::MWeapon_Halberd::~MWeapon_Halberd()
{
}

HRESULT Client::MWeapon_Halberd::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::MWeapon_Halberd::Initialize(void* arg)
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

	(pDesc)->wstrName = L"Monster_Weapon_Halberd" + to_wstring(namenum++);

	CHECK_FAILED(__super::Initialize(pDesc), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	Set_AllPass_VecObjPass(10);

	return S_OK;
}

_int Client::MWeapon_Halberd::Update_Priority(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Priority(fTimeDelta));

	return 0;
}

_int Client::MWeapon_Halberd::Update_Parallel(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Parallel(fTimeDelta));
	return 0;
}

_int Client::MWeapon_Halberd::Update(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update(fTimeDelta));

	return 0;
}

_int Client::MWeapon_Halberd::Update_Late(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Late(fTimeDelta));

	if (m_bIsVisible == false)
		return -1;

	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::SHADOW, this);

	return 0;
}

HRESULT Client::MWeapon_Halberd::Render(const _float fTimeDelta)
{
	if (m_bIsVisible == false)
		return S_OK;

	Monster_Weapon::Render(fTimeDelta);

	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		_uint bitflag = 0;
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &bitflag), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitflag), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));

		/* 쉐이더의 n번 패스를 사용해서 그리겠다고 선언 */
		CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);

		/* n번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT Client::MWeapon_Halberd::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
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

	for (size_t i = 0; i < iNumMeshes; i++)
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

HRESULT Client::MWeapon_Halberd::Ready_Components()
{
	CHECK_FAILED(Add_Shader(Proto_Shader(L"VTXMesh")), E_FAIL);
	CHECK_FAILED(Add_Model(m_wstrModelName), E_FAIL);

	// Dissolve용 텍스처 컴포넌트 추가
	CHECK_FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), Proto_Texture("Dissolve"),
		L"Com_DissolveTexture", RCAST(Component**)(&m_pNoiseTextureCom)), E_FAIL);

	Set_VecObjPassSize(m_pModelCom->Get_NumMeshes());

	return S_OK;
}

HRESULT Client::MWeapon_Halberd::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

	return S_OK;
}


MWeapon_Halberd* Client::MWeapon_Halberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	MWeapon_Halberd* pInstance = new MWeapon_Halberd(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"MWeapon_Halberd 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


GameObject* Client::MWeapon_Halberd::Clone(void* pArg)
{
	MWeapon_Halberd* pInstance = new MWeapon_Halberd(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"MWeapon_Halberd 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::MWeapon_Halberd::Free()
{
	__super::Free();
}
