#include "Engine_Define.h"
#include "ImguiRTV.h"
#include "GameInstance.h"
#include "Mouse.h"
#include "RenderTargetManager.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
ImguiRTV::ImguiRTV(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :ImguiWindow(pDevice, pContext), m_pMouse(Mouse::GetInstance())
{
}

HRESULT Engine::ImguiRTV::Initialize(void* pArg)
{
    ImguiWindow::IMGUIWINDOW_DESC ShaderImguiDesc;
    ShaderImguiDesc.m_WindowTitle = "RTVImguiWindow";
    m_pGameInstance = GameInstance::GetInstance();
    __super::Initialize(&ShaderImguiDesc);
    //ImGui::DockSpaceOverViewport();

    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_HDR));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Diffuse));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Normal));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_U));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Shade));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Specular));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_MtrlSpecular));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Roughness));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Shadow1));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Shadow_Bake));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_SSAO));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_SSAOBLur));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Emissive));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_BlurHorizontal));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_BlurVertical));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Bright));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_GodRayCopy));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_GodRay));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_VelocityOut));
    m_pSRVs.push_back(m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_CamMotionBlur));

    m_pRTVName.push_back("HDR");
    m_pRTVName.push_back("Diffuse");
    m_pRTVName.push_back("Normal");
    m_pRTVName.push_back("PBR");
    m_pRTVName.push_back("Shade");
    m_pRTVName.push_back("Specular");
    m_pRTVName.push_back("MtrlSpecular");
    m_pRTVName.push_back("Roughness");
    m_pRTVName.push_back("Shadow");
    m_pRTVName.push_back("Shadow_Bake");
    m_pRTVName.push_back("SSAO");
    m_pRTVName.push_back("SSAOBlur");
    m_pRTVName.push_back("Emissive");
    m_pRTVName.push_back("BlurHorizon");
    m_pRTVName.push_back("BlurVertical");
    m_pRTVName.push_back("Bright");
    m_pRTVName.push_back("GodRayCopy");
    m_pRTVName.push_back("GodRay");
    m_pRTVName.push_back("Velocity");
    m_pRTVName.push_back("MotionBlur");
    m_vSize = { 250.f, 250.f };
    return S_OK;
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 ////////////////////////////////////////////////////////
_uint Engine::ImguiRTV::Update_Contents(_float fTimeDelta)
{
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

    _float4 Campos = m_pGameInstance->Get_CameraPosition();
    if (ImGui::BeginTabBar("RTV_Debug", tab_bar_flags))
    {

        if (ImGui::BeginTabItem("RTV"))
        {
            Debug_RTV();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::Separator();

    return 0;
}
/******************************************************* 업데이트 *******************************************************/



//////////////////////////////////////////////////////// RTV 띄우기 ////////////////////////////////////////////////////////
void Engine::ImguiRTV::Debug_RTV()
{   
    for (int i = 0; i < m_pSRVs.size(); i++)
    {
        ImGui::BeginGroup();
        ImGui::Text(m_pRTVName[i]);
        ImGui::Image(m_pSRVs[i], m_vSize); 
        ImGui::EndGroup();
        
        if (!(i == 3 || i == 7 || i == 11 || i == 15)) ImGui::SameLine();
    }
        
}
/******************************************************* RTV 띄우기 *******************************************************/



//////////////////////////////////////////////////////// 생성자////////////////////////////////////////////////////////
ImguiRTV* ImguiRTV::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    ImguiRTV* pInstance = new ImguiRTV(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : ImguiRTV");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void Engine::ImguiRTV::Free()
{
    __super::Free();
}
/******************************************************* 생성자 *******************************************************/