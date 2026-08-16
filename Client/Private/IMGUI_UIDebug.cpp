#include "Client_Define.h"
#include "IMGUI_UIDebug.h"
#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
IMGUI_UIDebug::IMGUI_UIDebug()
{
}

IMGUI_UIDebug::IMGUI_UIDebug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:ImguiWindow(pDevice,pContext)
{
	m_pGameInstance = GameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

IMGUI_UIDebug::~IMGUI_UIDebug()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::IMGUI_UIDebug::Initialize(void* arg)
{
	m_WindowTitle = "UIDebugger";

	ImGui::SetNextWindowSize(ImVec2(300.0f, 400.0f), ImGuiCond_FirstUseEver);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/


_uint IMGUI_UIDebug::Update_Contents(_float fTimeDelta)
{

	//if(ImGui::Button(""))
	return 0;
}


//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
_int Client::IMGUI_UIDebug::Render(const _float fTimeDelta)
{
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/


//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
IMGUI_UIDebug* Client::IMGUI_UIDebug::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* arg)
{
	IMGUI_UIDebug* pInstance = new IMGUI_UIDebug(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(arg), L"IMGUI_UIDebug 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/




void IMGUI_UIDebug::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
