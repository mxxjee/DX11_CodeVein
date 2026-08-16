#include "Client_Define.h"
#include "Camera_Cinematic.h"

//#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Camera_Cinematic::Camera_Cinematic()
{
}

Client::Camera_Cinematic::Camera_Cinematic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Camera(pDevice, pContext)
{
}

Client::Camera_Cinematic::Camera_Cinematic(const Camera_Cinematic& original)
	: Camera(original)
{
}

Client::Camera_Cinematic::~Camera_Cinematic()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Camera_Cinematic::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Camera_Cinematic::Initialize(void* arg)
{
	static _uint namenum = 0;

	CAMERA_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(CAMERA_DESC*)(arg)->wstrName = L"Camera_Cinematic_" + to_wstring(namenum++);

	CHECK_FAILED(Camera::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Camera_Cinematic::Ready_Components()
{
	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Camera_Cinematic::Update_Priority(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return -1;


	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Camera_Cinematic::Update(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return -1;

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 카메라 전용 업데이트 ////////////////////////////////////////////////////////
void Client::Camera_Cinematic::Update_Camera(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	if (m_bIsChanged == true)
	{

		m_bIsChanged = false;
	}

}
/******************************************************* 카메라 전용 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Camera_Cinematic::Update_Late(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return -1;

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Camera_Cinematic::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);


	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::Camera_Cinematic::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Camera_Cinematic* Client::Camera_Cinematic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Camera_Cinematic* pInstance = new Camera_Cinematic(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Camera_Cinematic 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Camera_Cinematic::Clone(void* pArg)
{
	Camera_Cinematic* pInstance = new Camera_Cinematic(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Camera_Cinematic 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Camera_Cinematic::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
