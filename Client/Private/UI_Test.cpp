#include "Client_Define.h"
#include "UI_Test.h"
#include "UI_Image.h"


//#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::UI_Test::UI_Test()
{
}

Client::UI_Test::UI_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: UIObject(pDevice, pContext)
{
}

Client::UI_Test::UI_Test(const UI_Test& original)
	: UIObject(original)
{
}

Client::UI_Test::~UI_Test()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UI_Test::Initialize_Prototype(LEVEL _level)
{
	UIObject::Initialize_Prototype(_UINT(_level));

	return S_OK;
}

HRESULT Client::UI_Test::Initialize(void* arg)
{
	static _uint namenum = 0;
	m_iNumber = namenum;
	
#pragma region 부모자식테스트(임시 텍스처바인딩)
	wstring TexKey = L"";
	switch (m_iNumber)
	{
	case 0:
		TexKey = Proto_UITexture(L"Test1");
		break;

	case 1:
		TexKey = Proto_UITexture(L"Test2");
		break;
	case 2:
		TexKey = Proto_UITexture(L"Test1_1");
		break;

	case 3:
		TexKey = Proto_UITexture(L"Test1_2");
		break;
	}

#pragma endregion

	UIOBJECT_DESC desc;
	
	if (arg == nullptr)
	{
		/*기본값*/
		desc.fCX = 100.f;
		desc.fCY = 100.f;
		desc.fX = 600.f + namenum * 200.f;
		desc.fY = 200.f;
		//desc.bIsPickable = true;

		arg = &desc;
	}
	else
	{
		UIOBJECT_DESC* UIArg = CAST(UIOBJECT_DESC*)(arg);
		//UIArg->bIsPickable = true;

	}

	CAST(UIOBJECT_DESC*)(arg)->wstrName = L"UI_Test_" + to_wstring(namenum++);
	
	CHECK_FAILED(UIObject::Initialize(arg), E_FAIL);

	UI_Image::UIIMAGEDESC Desc;
	Desc.pOwner = this;
	Desc.TextureComponentKey = TexKey;

	Add_Component(0, Proto_UIImage, Proto_UIImage, RCAST(Component**)(&m_pUIRenderer),&Desc);

	
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
//HRESULT Client::UI_Test::Ready_Components()
//{
//	UIObject::Ready_Components(Proto_UITexture(L"White"));
//
//	return S_OK;
//}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::UI_Test::Update_Priority(const _float fTimeDelta)
{



	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::UI_Test::Update(const _float fTimeDelta)
{
	
	

#pragma region lerp함수 시연
	//static _float Timer = {};

	//_float duration = 3.f;
	//Timer += fTimeDelta;
	//if (Timer >= duration + 2.f)
	//{
	//	Timer = 0.f;
	//}

	//_float ratio = Timer / duration;
	//if (ratio >= 1.f)
	//{
	//	ratio = 1.f;
	//}

	//switch (m_iNumber)
	//{
	//case 0:
	//	m_fY = fLerp(200.f, 700.f, (ratio));
	//	m_bIsVisible = true;
	//	break;

	//case 1:
	//	m_fY = fLerp(200.f, 700.f, Pulse(ratio));
	//	m_bIsVisible = true;
	//	break;

	//case 2:
	//	m_fY = fLerp(200.f, 700.f, EaseOutBounce(ratio));
	//	m_bIsVisible = true;
	//	break;
	//}
#pragma endregion lerp함수 시연


#pragma region SinWave 시연
	static _float Timer = {};
	Timer += fTimeDelta;


	//switch (m_iNumber)
	//{
	///*case 0:
	//	m_fSizeX = m_fSizeY = SinWave(Timer, 1.f, 100.f, 200.f);
	//	m_bIsVisible = true;
	//	break;*/
	////case 1:
	////	m_fSizeX = m_fSizeY = 100.f + Heartbeat(Timer, 20.f) * 100.f;
	////	m_bIsVisible = true;
	////	break;
	////case 2:
	////	_float size = clamp(Breathe(Timer, 1.f) * 200.f, 100.f, 200.f);
	////	m_fSizeX = m_fSizeY = size;
	////	m_bIsVisible = true;
	////	break;
	//}
#pragma endregion SinWave 시연




	__super::Update(fTimeDelta);

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::UI_Test::Update_Late(const _float fTimeDelta)
{
	UIObject::Update_Late(fTimeDelta);

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::UI_Test::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/





//////////////////////////////////////////////////////// 피킹 함수 ////////////////////////////////////////////////////////
void Client::UI_Test::OnClick()
{
	COUT("**************************클릭*************************");
}
void Client::UI_Test::OnHover()
{
}
void Client::UI_Test::OnHoverEnter()
{
	COUT("**************************호버시작*************************");
}
void Client::UI_Test::OnHoverExit()
{
	COUT("**************************호버끝*************************");
}
/******************************************************* 피킹 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
UI_Test* Client::UI_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	UI_Test* pInstance = new UI_Test(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"UI_Test 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::UI_Test::Clone(void* pArg)
{
	UI_Test* pInstance = new UI_Test(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UI_Test 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::UI_Test::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
