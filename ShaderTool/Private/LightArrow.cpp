#include "ShaderTool_Define.h"
#include "LightArrow.h"
#include "Texture.h"
#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
ShaderTool::LightArrow::LightArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{
}

ShaderTool::LightArrow::LightArrow(const LightArrow& original)
	: GameObject(original)
{
}

/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::LightArrow::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT ShaderTool::LightArrow::Initialize(void* arg)
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

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"LightArrow_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::LightArrow::Ready_Components()
{
	//게임오브젝트에서 복사생성
	if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), Proto_Com_VIRect,
		L"Com_VIBuffer", reinterpret_cast<Component**>(&m_pVIBuffer))))
		return E_FAIL;

	//쉐이더
	if (FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), Proto_Shader(L"LightArrow"),
		L"Com_Shader", reinterpret_cast<Component**>(&m_pShaderCom))))
		return E_FAIL;

	//텍스쳐 만들기
	if (FAILED(__super::Add_Component(_UINT(LEVEL::LOGO), Proto_Texture(L"LightArrow"),
		L"Com_Texture", reinterpret_cast<Component**>(&m_pTexture))))
		return E_FAIL;

	Set_VecObjPassSize();
	Set_AllPass_VecObjPass();

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::LightArrow::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::LightArrow::Update(const _float fTimeDelta)
{
	//카메라 위치 가져와서 옆에뜨게
	//벡터의 합이다
	_float4x4 CamMatrix = m_pGameInstance->Get_PipeLineInversMatrix(D3DTRANSFORM::D3DTS_VIEW);
	_vector right = XMVectorSet(CamMatrix._11, CamMatrix._12, CamMatrix._13, CamMatrix._14);
	_vector up = XMVectorSet(CamMatrix._21, CamMatrix._22, CamMatrix._23, CamMatrix._24);
	_vector look = XMVectorSet(CamMatrix._31, CamMatrix._32, CamMatrix._33, CamMatrix._34);
	_vector posistion = XMVectorSet(CamMatrix._41, CamMatrix._42, CamMatrix._43, CamMatrix._44);
	
	_vector posistion1 = posistion + look * 5.f + right + up;
	m_pTransformCom->Set_State(DIRECTION::POSITION, posistion1);

	//화살표 따라가는거(뭘해도 이상하다)
	LIGHT_DESC* desc = m_pGameInstance->Get_LightDesc(0);
	XMVECTOR pos = XMLoadFloat4(&desc->vDirection);
	XMVECTOR dir = XMVector3Normalize(pos);
	XMVECTOR DefaultLook = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	XMVECTOR axis = XMVector3Cross(DefaultLook, dir);
	axis = XMVector3Normalize(axis);
	float angle = acosf(XMVectorGetX(XMVector3Dot(DefaultLook, dir)));
	if (XMVector4Equal(axis, XMVectorSet(0.f, 0.f, 0.f, 0.f)) || angle == 0)
		return 0;

	m_pTransformCom->Rotation(axis, angle);
	/*XMVECTOR quat = XMQuaternionRotationAxis(axis, angle);
	quat = XMQuaternionNormalize(quat);*/
	/*_vector vright = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vector vup = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vlook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	vright = XMVector3Rotate(vright, quat);
	vup = XMVector3Rotate(vup, quat);
	vlook = XMVector3Rotate(vlook, quat);

	Set_State(DIRECTION::RIGHT, vright);
	Set_State(DIRECTION::UP, vup);
	Set_State(DIRECTION::LOOK, vlook);*/
	
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::LightArrow::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::LightArrow::Render(const _float fTimeDelta)
{
	m_pGameInstance->Set_RasterizerState(RS_SolidNone);

	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);
		
	//비긴(CPU에 있는거랑 HLSL에 있는거랑 정점정보들 연결) 적용
	if (FAILED(m_pShaderCom->Begin(m_vecObjPass[0])))
		return E_FAIL;

	//공통된 쉐이더 b를 던져준다
	CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[0]), E_FAIL);

	//버퍼 바인드 리소스(장치에 버퍼,인덱스 설정)
	if (FAILED(m_pVIBuffer->Bind_Resource()))
		return E_FAIL;

	//버퍼렌더
	if (FAILED(m_pVIBuffer->Render(fTimeDelta)))
		return E_FAIL;

	return S_OK;

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::LightArrow::Bind_ShaderResources()
{
	//월드 세팅
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

	//보이긴하는데 찌그러져 보인다 VIBuffer쪽 봐야될듯
	CHECK_FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTRANSFORM::D3DTS_VIEW), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTRANSFORM::D3DTS_PROJ), E_FAIL);

	//specular위해서 캠위치 던진다
	CHECK_FAILED(m_pGameInstance->Bind_CameraPosition(m_pShaderCom, "g_vCamPosition"), E_FAIL);

	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
LightArrow* ShaderTool::LightArrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	LightArrow* pInstance = new LightArrow(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"LightArrow 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* ShaderTool::LightArrow::Clone(void* pArg)
{
	LightArrow* pInstance = new LightArrow(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"LightArrow 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void ShaderTool::LightArrow::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
