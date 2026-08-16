#include "Client_Define.h"
#include "Sky_Sphere.h"

//#include "GameInstance.h"
#include "VIBuffer_SkySphere.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Sky_Sphere::Sky_Sphere()
{
}

Client::Sky_Sphere::Sky_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{
}

Client::Sky_Sphere::Sky_Sphere(const Sky_Sphere& original)
	: GameObject(original)
{
}

Client::Sky_Sphere::~Sky_Sphere()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sky_Sphere::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Sky_Sphere::Initialize(void* arg)
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

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Sky_Sphere_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	//m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sky_Sphere::Ready_Components()
{
	Add_Component(_UINT(LEVEL::STATIC), Proto_Com_Shader_Sky, Com_Shader, RCAST(Component**)(&m_pShaderCom));	// 쉐이더
	if (m_pGameInstance->Get_Current_LevelID() == _UINT(LEVEL::CUSTOMIZE))
	{
		Add_Component(_UINT(LEVEL::CUSTOMIZE), Proto_Texture(L"Sky_Sphere_Customize"), Com_Texture, RCAST(Component**)(&m_pTextureCom));	// 텍스쳐
		Add_Component(_UINT(LEVEL::CUSTOMIZE), Proto_Com_VISkySphere, Com_VIBuffer, RCAST(Component**)(&m_pVIBufferCom));	// 스카이 스피어
		m_iSkyTextureNum = 7;

	}
	else
	{
		Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"Sky_Sphere"), Com_Texture, RCAST(Component**)(&m_pTextureCom));	// 텍스쳐
		Add_Component(_UINT(LEVEL::STATIC), Proto_Com_VISkySphere, Com_VIBuffer, RCAST(Component**)(&m_pVIBufferCom));	// 스카이 스피어
	}


	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sky_Sphere::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sky_Sphere::Update(const _float fTimeDelta)
{
	if (m_pGameInstance->Get_Current_LevelID() != _UINT(LEVEL::CUSTOMIZE))
		return 0;

	if (m_pGameInstance->KeyDown(DIK_NUMPAD1))
	{
		m_iSkyTextureNum = 0;
	}
	else if (m_pGameInstance->KeyDown(DIK_NUMPAD2))
	{
		m_iSkyTextureNum = 1;
	}
	else if (m_pGameInstance->KeyDown(DIK_NUMPAD3))
	{
		m_iSkyTextureNum = 2;
	}
	else if (m_pGameInstance->KeyDown(DIK_NUMPAD4))
	{
		m_iSkyTextureNum = 3;
	}
	else if (m_pGameInstance->KeyDown(DIK_NUMPAD5))
	{
		m_iSkyTextureNum = 4;
	}
	else if (m_pGameInstance->KeyDown(DIK_NUMPAD6))
	{
		m_iSkyTextureNum = 5;
	}
	else if (m_pGameInstance->KeyDown(DIK_NUMPAD7))
	{
		m_iSkyTextureNum = 6;
	}

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sky_Sphere::Update_Late(const _float fTimeDelta)
{
	Add_RenderGroup(RENDER_GROUP::PRIORITY);


	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sky_Sphere::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);


	m_pShaderCom->Begin(0);

	m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 0, m_iSkyTextureNum);

	m_pVIBufferCom->Bind_Resource();

	m_pShaderCom->Commit(0);

	m_pVIBufferCom->Render(0.f);


	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sky_Sphere::Bind_ShaderResources()
{
	m_pTransformCom->Set_State(DIRECTION::POSITION, m_pGameInstance->Get_CameraPosition());
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);
	//m_pGameInstance->Bind_PipeLine_All(m_pShaderCom);
	//GameObject::Bind_ProjectionMatrices(m_pShaderCom);
	//m_pGameInstance->Bind_PipeLine_All(m_pShaderCom);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Sky_Sphere* Client::Sky_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Sky_Sphere* pInstance = new Sky_Sphere(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Sky_Sphere 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Sky_Sphere::Clone(void* pArg)
{
	Sky_Sphere* pInstance = new Sky_Sphere(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Sky_Sphere 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Sky_Sphere::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
