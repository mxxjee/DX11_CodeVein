#include "Engine_Define.h"
#include "UI_EventReactor.h"
#include "GameInstance.h"
#include "UIObject.h"

Engine::UI_EventReactor::UI_EventReactor()
    :UIComponent()
{
}

Engine::UI_EventReactor::UI_EventReactor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIComponent(pDevice,pContext)
{
}

Engine::UI_EventReactor::UI_EventReactor(const UI_EventReactor& original)
    :UIComponent(original)
{
}

Engine::UI_EventReactor::~UI_EventReactor()
{
}

HRESULT Engine::UI_EventReactor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Engine::UI_EventReactor::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    if (m_pOwner)
    {
      /*  if (false == m_pOwner->Is_Active())
            return E_FAIL;*/

    }

    m_Handle=m_pGameInstance->Subscribe<UI_MasterEvent>([this](const UI_MasterEvent& e)
        {
            if (e.m_iObjID != m_pOwner->Get_ObjectID())
                return;

            for (auto& bind : m_Bindings)
            {
                if (e.m_EventTarget == UI_EVENT_TARGET::SYSTEM)
                    continue;

                if (e.m_ActionName != bind.m_ResponseAction)
                    continue;

                if (e.m_EventTarget == UI_EVENT_TARGET::COMPONENT)
                {

                    Component* pComp = m_pOwner->Get_Component_FromName(bind.m_ReponseComponent);
                    if (!pComp)
                        continue;

                    UIComponent* pUIComp = dynamic_cast<UIComponent*>(pComp);
                    if (pUIComp)
                        pUIComp->Execute(bind.m_ResponseAction, (void*)&e);

                }


                else
                {
                    if (m_pOwner)
                        m_pOwner->Execute_By_Event(bind.m_ResponseAction, (void*)&e);
                }


                
            }
        });
    return S_OK;
}

void Engine::UI_EventReactor::Add_Binding(const EventBinding& eBinding)
{
    m_Bindings.push_back(eBinding);
}







#ifdef _DEBUG
void Engine::UI_EventReactor::Create_Binding()
{
    UI_EventReactor::EventBinding eventBoudning;
    m_Bindings.push_back(eventBoudning);

}

void Engine::UI_EventReactor::Render_Imgui()
{
    //Imgui창만들기
    //현재 바인딩리스트
    Open_BindingList();

    ImGui::Separator();
   
    //바인딩 생성하는 버튼
    if (ImGui::Button("Create BindingEvent"))
    {
        Create_Binding();
      
    }



}

void Engine::UI_EventReactor::Open_BindingList()
{
    for (size_t i = 0; i < m_Bindings.size(); ++i)
    {
        auto& bind = m_Bindings[i];

        
        char treeLabel[128];
        sprintf_s(treeLabel, "Binding [%d] : %s###BindingNode_%d",
            (int)i, bind.m_ResponseAction.c_str(), (int)i);

 
        if (ImGui::TreeNode(treeLabel))
        {

            // Action
            char szAction[256];
            strcpy_s(szAction, bind.m_ResponseAction.c_str());
            if (ImGui::InputText("Action Name", szAction, 256)) {
                bind.m_ResponseAction = szAction;
            }

            //Target Component
            char szComp[256];
            WideCharToMultiByte(CP_ACP, 0, bind.m_ReponseComponent.c_str(), -1, szComp, 256, NULL, NULL);
            if (ImGui::InputText("Target Component", szComp, 256)) {
                wchar_t wComp[256];
                MultiByteToWideChar(CP_ACP, 0, szComp, -1, wComp, 256);
                bind.m_ReponseComponent = wComp;
            }

         

            // 삭제 버튼
            if (ImGui::Button("Remove Binding", ImVec2(-1, 0))) {
                m_Bindings.erase(m_Bindings.begin() + i);
                ImGui::TreePop();
                break;
            }

            ImGui::TreePop();
        }
    }
}
#endif

UI_EventReactor* Engine::UI_EventReactor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    UI_EventReactor* pInstance = new UI_EventReactor(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(), L"UI_EventReactor 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Component* Engine::UI_EventReactor::Clone(void* arg)
{
    UI_EventReactor* pInstance = new UI_EventReactor(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"UI_EventReactor 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}



void Engine::UI_EventReactor::Free()
{
   /* if(m_bIsClone)
        m_pGameInstance->UnsubScribe(m_Handle);
  */  __super::Free();
}

#pragma region parsing
void Engine::UI_EventReactor::Save_Data(ordered_json& pJson)
{
    pJson["Bindings"] = ordered_json::array();

    for (auto& binding : m_Bindings)
    {
        pJson["Bindings"].push_back(binding.To_Json());
    }
    
}

void Engine::UI_EventReactor::Load_Data(GameObject* pOwner, ComponentData& Data, UITYPE* eType)
{
    __super::Load_Data(pOwner, Data, eType);
    CHECK_JUST_NULL(pOwner);

    ordered_json root = Data.ComJson;


    if (root.contains("Bindings") && root["Bindings"].is_array())
    {
        for (auto& pData : root["Bindings"])
        {
            EventBinding Binding;
            Binding.From_Json(pData);
            m_Bindings.push_back(Binding);

        }
    }

    *eType = UITYPE::EVENTREACTOR;
}

#pragma endregion