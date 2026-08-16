#include "MT_Defines.h"
#include "Layer_Debug_UI.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Camera_Free.h"
#include "Light.h"
#include "StaticObj.h"

CLayer_Debug_UI::CLayer_Debug_UI(ID3D11Device* pD, ID3D11DeviceContext* pC)
    :ImguiWindow(pD, pC)
{
}

HRESULT CLayer_Debug_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pGameInstance = GameInstance::GetInstance();
    if (nullptr != m_pGameInstance)
        Safe_AddRef(m_pGameInstance);

    return S_OK;
}

_uint CLayer_Debug_UI::Update_Contents(_float fDT)
{
    auto& Layers = m_pGameInstance->Get_Current_Layers();

    if (g_pSelectedObject != nullptr && ImGui::IsKeyPressed(ImGuiKey_Delete) && !ImGui::GetIO().WantTextInput)
    {
        g_pSelectedObject->Set_Dead(true);
    }
    // 전역 선택 오브젝트 유효성 검사
    if (g_pSelectedObject && g_pSelectedObject->Is_Dead())
    {
        CStaticObj* pStatic = dynamic_cast<CStaticObj*>(g_pSelectedObject);
        if (pStatic) pStatic->Set_Hovered(false);
        Safe_Release(g_pSelectedObject);
        g_pSelectedObject = nullptr;
    }

    ImGui::BeginChild("ObjectBrowser", ImVec2(0, -100), true);
    ImGui::Text("Layer List");
    ImGui::Separator();

    for (auto& pairLayer : Layers)
    {
        _string strLayerName;
        strLayerName.assign(pairLayer.first.begin(), pairLayer.first.end());
        
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
        if (strLayerName != "Layer_Object" && strLayerName != "Layer_Static" && strLayerName != "Layer_Function" && strLayerName != "Layer_Enviroment")
            nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

        if (ImGui::TreeNodeEx(strLayerName.c_str(), nodeFlags))
        {
            Layer* pLayer = pairLayer.second;
            auto& GameObjectsMap = pLayer->Get_GameObjects();

            vector<pair<_wstring, GameObject*>> vecValidObjects;
            vecValidObjects.reserve(GameObjectsMap.size());
            for (auto& pairObj : GameObjectsMap)
            {
                if (!pairObj.second->Is_Dead())
                    vecValidObjects.push_back(pairObj);
            }
            size_t curSize = vecValidObjects.size();
            if (curSize != m_LayerObjectCounts[strLayerName])
            {
                std::sort(vecValidObjects.begin(), vecValidObjects.end(),
                    [](const pair<_wstring, GameObject*>& a, const pair<_wstring, GameObject*>& b)
                    {
                        auto GetNum = [](const _wstring& str) -> int {
                            size_t pos = str.rfind(L'_');
                            if (pos == _wstring::npos) return 0;
                            try { return std::stoi(str.substr(pos + 1)); }
                            catch (...) { return 0; }
                            };
                        return GetNum(a.first) < GetNum(b.first);
                    });
                m_LayerObjectCounts[strLayerName] = curSize;
            }

            ImGuiListClipper clipper;
            clipper.Begin((int)vecValidObjects.size());
            while (clipper.Step())
            {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
                {
                    GameObject* pGameObject = vecValidObjects[i].second;
                    _string strObjKey;
                    strObjKey.assign(vecValidObjects[i].first.begin(), vecValidObjects[i].first.end());

                    ImGui::PushID(pGameObject);

                    // 렌더링 활성화/비활성화
                    bool bIsVisible = pGameObject->Is_Visible();
                    if (ImGui::Checkbox(("##Vis_" + strObjKey).c_str(), &bIsVisible))
                        pGameObject->Set_Visible(bIsVisible);

                    ImGui::SameLine();

                    if (ImGui::Selectable(strObjKey.c_str(), g_pSelectedObject == pGameObject))
                    {
                        CStaticObj* pPrev = dynamic_cast<CStaticObj*>(g_pSelectedObject);
                        if (pPrev) pPrev->Set_Hovered(false);
                        Safe_Release(g_pSelectedObject);

                        g_pSelectedObject = pGameObject;
                        Safe_AddRef(g_pSelectedObject);

                        CStaticObj* pNew = dynamic_cast<CStaticObj*>(g_pSelectedObject);
                        if (pNew) pNew->Set_Hovered(true);
                    }

                    // 특정 오브젝트 삭제
                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Delete Object"))
                        {
                            if (g_pSelectedObject == pGameObject)
                            {
                                CStaticObj* pStatic = dynamic_cast<CStaticObj*>(g_pSelectedObject);
                                if (pStatic) pStatic->Set_Hovered(false);
                                Safe_Release(g_pSelectedObject);
                                g_pSelectedObject = nullptr;
                            }
                            pGameObject->Set_Dead(true);
                        }
                        ImGui::EndPopup();
                    }

                    ImGui::PopID();
                }
            }
            ImGui::TreePop();
        }
    }
    ImGui::EndChild();

    if (ImGui::Button("Delete All Object", ImVec2(ImGui::GetContentRegionAvail().x, 30)))
    {
    }

    ImGui::BeginChild("LightEditor", ImVec2(0, 200), true);
    ImGui::Text("Light Manager");
    ImGui::Separator();

    auto pLightMap = m_pGameInstance->Get_LightuMapPtr();
    for (auto& pair : *pLightMap)
    {
        _uint iLightIndex = pair.first;
        Light* pLight = pair.second;
        LIGHT_DESC* pDesc = pLight->Get_LightDesc();

        ImGui::PushID(iLightIndex);
        _string strLightName = "Light [" + to_string(iLightIndex) + "]";

        if (ImGui::CollapsingHeader(strLightName.c_str()))
        {
            ImGui::DragFloat3("Position", (float*)&pDesc->vPosition, 0.1f);
            ImGui::DragFloat3("Direction", (float*)&pDesc->vDirection, 0.01f);
            ImGui::ColorEdit4("Diffuse", (float*)&pDesc->vDiffuse);
            ImGui::ColorEdit4("Ambient", (float*)&pDesc->vAmbient);
            ImGui::ColorEdit4("Specular", (float*)&pDesc->vSpecular);

            if (ImGui::Button("Delete Light"))
            {
                m_pGameInstance->Delete_Light(iLightIndex);
                ImGui::PopID();
                break;
            }
        }
        ImGui::PopID();
    }
    ImGui::EndChild();

    ImGui::BeginChild("Camera Controler", ImVec2(0, 0), true);
    {
        ImGui::Separator();
        ImGui::Text("Camera Quick Control");

        static _float3 vTargetPos = { 0.f, 0.f, 0.f };
        ImGui::InputFloat3("Move To", (float*)&vTargetPos);

        if (ImGui::Button("Teleport to Object"))
        {
            if (g_pSelectedObject != nullptr) 
            {
                auto pCamera = (CCamera_Free*)m_pGameInstance->Get_Camera(_UINT(CAMERA::FREE));
                if (pCamera)
                {
                    _vector vPos = g_pSelectedObject->Get_Position();
                    vPos = XMVectorSetW(vPos, 1.f);
                    pCamera->Set_CameraPosition(vPos);
                }
            }
        }

        if (ImGui::Button("Teleport Camera"))
        {
            auto pCamera = (CCamera_Free*)m_pGameInstance->Get_Camera(_UINT(CAMERA::FREE));
            if (pCamera)
            {
                _vector vPos = XMLoadFloat3(&vTargetPos);
                vPos = XMVectorSetW(vPos, 1.f);
                pCamera->Set_CameraPosition(vPos);
            }
        }

        if (ImGui::Button("Reset Camera (Origin)"))
        {
            auto pCamera = (CCamera_Free*)m_pGameInstance->Get_Camera(_UINT(CAMERA::FREE));
            if (pCamera)
                pCamera->Set_CameraPosition(XMVectorSet(0.f, 10.f, -10.f, 1.f));
        }
    }
    ImGui::EndChild();

    return 0;
}

CLayer_Debug_UI* CLayer_Debug_UI::Create(ID3D11Device* pD, ID3D11DeviceContext* pC, void* pArg)
{
    CLayer_Debug_UI* pIns = new CLayer_Debug_UI(pD, pC);
    if (FAILED(pIns->Initialize(pArg)))
    {
        MSG_BOX("imgui_Creater 생성 실패"); Safe_Release(pIns);
    }
    return pIns;
}

void CLayer_Debug_UI::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}