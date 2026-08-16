#include "Engine_Define.h"
#include "Picking.h"
#include "GameInstance.h"
#include "RenderTargetManager.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Picking::Picking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }, m_pGameInstance{ GameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

Engine::Picking::~Picking()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////

HRESULT Engine::Picking::Initialize(HWND hWnd, _uint iWidth, _uint iHeight)
{
	D3D11_TEXTURE2D_DESC		TextureDesc{};
	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_STAGING;
	TextureDesc.BindFlags = 0;

	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	m_pPickInfo = new _float4[iWidth * iHeight]; //화면크기로 만들고
	ZeroMemory(m_pPickInfo, sizeof(_float4) * iWidth * iHeight);

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	m_hWnd = hWnd;

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Picking::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Picking::Update()
{
	m_pGameInstance->Copy_Resource(RenderTargets::Target_Pick, m_pTexture2D); //화면복사

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource); //텍스쳐2D SubResource에 정보담고

	memcpy(m_pPickInfo, SubResource.pData, sizeof(_float4) * m_iWidth * m_iHeight); //피킹에 던진다

	m_pContext->Unmap(m_pTexture2D, 0);	//닫기

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Picking::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 피킹 함수 ////////////////////////////////////////////////////////
_bool Engine::Picking::PickingObject(_float4* pOut)
{
	Update();

	::POINT		ptMouse{};

		GetCursorPos(&ptMouse);

	ScreenToClient(m_hWnd, &ptMouse); //마우스 커서 뷰포트사이즈로 맞추고

	_uint		iIndex = ptMouse.y * m_iWidth + ptMouse.x; //픽셀 index찾는거

	if (iIndex >= m_iWidth * m_iHeight ||  //화면을 벗어나거나 w값이 0이라면 실패
		0.f == m_pPickInfo[iIndex].w)
		return false; 

	*pOut = m_pPickInfo[iIndex]; //화면의 픽셀의 float값을 넘겨준다

	return true;
}
/******************************************************* 피킹 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Picking* Engine::Picking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWidth, _uint iHeight)
{
	Picking* pInstance = new Picking(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(hWnd, iWidth, iHeight), L"Picking 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Picking::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pPickInfo);

	Safe_Release(m_pTexture2D);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
/******************************************************* 객체 반환 함수 *******************************************************/
