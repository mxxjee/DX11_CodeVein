#include "ShaderTool_Define.h"
#include "GodRay_Sun.h"

//#include "GameInstance.h"
#include "VIBuffer_SkySphere.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
ShaderTool::GodRay_Sun::GodRay_Sun()
{
}

ShaderTool::GodRay_Sun::GodRay_Sun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{
}

ShaderTool::GodRay_Sun::GodRay_Sun(const GodRay_Sun& original)
	: GameObject(original)
{
}

ShaderTool::GodRay_Sun::~GodRay_Sun()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::GodRay_Sun::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT ShaderTool::GodRay_Sun::Initialize(void* arg)
{
	static _uint namenum = 0;

	GodRayDesc* desc  = static_cast<GodRayDesc*>(arg);

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"GodRay_Sun_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);
	m_pTransformCom->Set_State(DIRECTION::POSITION, desc->vSunPos); //-122.29f, 105.36f, -153.458f // _float4{ -185.89, 95.86, -43.45, 1.f }
	m_pTransformCom->Rotation(XMConvertToRadians(0.f), XMConvertToRadians(90.f), XMConvertToRadians(0.f));
	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::GodRay_Sun::Ready_Components()
{
	Add_Component(_UINT(LEVEL::STATIC), Proto_Com_VISphere, Com_VIBuffer, RCAST(Component**)(&m_pVIBufferCom));	// 스카이 스피어
	Add_Component(_UINT(LEVEL::STATIC), Proto_Com_Shader_Sky, Com_Shader, RCAST(Component**)(&m_pShaderCom));	// 쉐이더
	Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"Sky_Sphere"), Com_Texture, RCAST(Component**)(&m_pTextureCom));	// 텍스쳐

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::GodRay_Sun::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::GodRay_Sun::Update(const _float fTimeDelta)
{

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::GodRay_Sun::Update_Late(const _float fTimeDelta)
{
	Add_RenderGroup(RENDER_GROUP::NONBLEND);


	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::GodRay_Sun::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);


	m_pShaderCom->Begin(1);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);
	m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 0, 1);

	m_pVIBufferCom->Bind_Resource();

	m_pShaderCom->Commit(1);

	m_pVIBufferCom->Render(0.f);


	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::GodRay_Sun::Bind_ShaderResources()
{
	//m_pTransformCom->Set_State(DIRECTION::POSITION, m_pGameInstance->Get_CameraPosition());
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);
	//m_pGameInstance->Bind_PipeLine_All(m_pShaderCom);
	//GameObject::Bind_ProjectionMatrices(m_pShaderCom);
	//m_pGameInstance->Bind_PipeLine_All(m_pShaderCom);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
GodRay_Sun* ShaderTool::GodRay_Sun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	GodRay_Sun* pInstance = new GodRay_Sun(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"GodRay_Sun 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* ShaderTool::GodRay_Sun::Clone(void* pArg)
{
	GodRay_Sun* pInstance = new GodRay_Sun(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"GodRay_Sun 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void ShaderTool::GodRay_Sun::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
