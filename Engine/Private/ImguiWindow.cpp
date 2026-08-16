#include "Engine_Define.h"
#include "ImguiWindow.h"

Engine::ImguiWindow::ImguiWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
}

HRESULT Engine::ImguiWindow::Initialize(void* pArg)
{
    IMGUIWINDOW_DESC* pDesc = static_cast<IMGUIWINDOW_DESC*>(pArg);
    if(pDesc)
        m_WindowTitle = pDesc->m_WindowTitle;



    return S_OK;
}

_int Engine::ImguiWindow::Update_Priority(const _float fTimeDelta)
{
    return 0;
}

_int Engine::ImguiWindow::Update(const _float fTimeDelta)
{
    if (!m_bOpen)
        return 0;

    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);
    Update_Contents(fTimeDelta);
    ImGui::End();

    return 0;
}

_int Engine::ImguiWindow::Render(const _float fTimeDelta)
{
    //디버그용렌더..
    return 0;
}

void Engine::ImguiWindow::Free()
{
    __super::Free();
}

