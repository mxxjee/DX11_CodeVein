#include "ShaderTool_Define.h"
#include "BossMap.h"
#include "Texture.h"
#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
ShaderTool::BossMap::BossMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{
}

ShaderTool::BossMap::BossMap(const BossMap& original)
	: GameObject(original)
{
}

/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::BossMap::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT ShaderTool::BossMap::Initialize(void* arg)
{
	static _uint namenum = 0;

	GAMEOBJECT_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"BossMap_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_pTransformCom->Set_State(DIRECTION::POSITION, XMVectorSet(3.f, 4.f, 0.f, 1.f));

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::BossMap::Ready_Components()
{

	//쉐이더
	if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXMesh,
		TEXT("Com_Shader"), reinterpret_cast<Component**>(&m_pShaderCom))))
		return E_FAIL;

	//모델
	if (FAILED(__super::Add_Component(_UINT(LEVEL::LOGO), TEXT("Prototype_Component_Model_Map"),
		TEXT("Com_Model"), reinterpret_cast<Component**>(&m_pModelCom))))
		return E_FAIL;

	Set_VecObjPassSize(m_pModelCom->Get_NumMeshes());
	Set_AllPass_VecObjPass();

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::BossMap::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::BossMap::Update(const _float fTimeDelta)
{

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::BossMap::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::SHADOW_BAKE, this);

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::BossMap::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);


	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	//m_pGameInstance->Set_RasterizerState(RS_SolidNone);

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		//쉐이더 클래스에 잠시저장?
		CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0), E_FAIL);

		//얘가 패스 정해주는거잖아
		CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);

		//공통된 쉐이더 b를 던져준다
		CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);

		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}


	return S_OK;
}
HRESULT ShaderTool::BossMap::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	//이걸 내일 갈아치워야겠네
	m_pShaderCom->Bind_RawValue_FullSlot(11, "g_ShadowCascadeNum", &iCascadeNum, sizeof(_int));
	m_pShaderCom->Bind_Matrix("g_ShadowCascade_ViewProjMatrix", m_pGameInstance->Get_LightMatrix(D3DTRANSFORM::D3DTS_VIEWPROJ));
		
	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	//m_pGameInstance->Set_RasterizerState(RS_SolidNone);

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		//쉐이더 클래스에 잠시저장?
		CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0), E_FAIL);

		//얘가 패스 정해주는거잖아
		CHECK_FAILED(m_pShaderCom->Begin(5, true), E_FAIL);

		//공통된 쉐이더 b를 던져준다
		CHECK_FAILED(m_pShaderCom->Bind_Resources(5), E_FAIL);

		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}


	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::BossMap::Bind_ShaderResources()
{
	//월드 세팅
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

	//보이긴하는데 찌그러져 보인다 VIBuffer쪽 봐야될듯
	CHECK_FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTRANSFORM::D3DTS_VIEW), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTRANSFORM::D3DTS_PROJ), E_FAIL);

	//specular위해서 캠위치 던진다
	CHECK_FAILED(m_pGameInstance->Bind_CameraPosition(m_pShaderCom, "g_vCamPosition"), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint)), E_FAIL);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
BossMap* ShaderTool::BossMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	BossMap* pInstance = new BossMap(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"BossMap 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* ShaderTool::BossMap::Clone(void* pArg)
{
	BossMap* pInstance = new BossMap(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"BossMap 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void ShaderTool::BossMap::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
