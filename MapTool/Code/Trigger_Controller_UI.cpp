#include "MT_Defines.h"
#include "Trigger_Controller_UI.h"
#include "GameInstance.h"
#include "Mouse.h"
#include "Monster_EventShape.h"

Trigger_Controller_UI::Trigger_Controller_UI(ID3D11Device* pD, ID3D11DeviceContext* pC)
    :ImguiWindow(pD, pC)
{
}

HRESULT Trigger_Controller_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pGameInstance = GameInstance::GetInstance();
    if (nullptr != m_pGameInstance)
        Safe_AddRef(m_pGameInstance);

    return S_OK;
}

_uint Trigger_Controller_UI::Update_Contents(_float fDT)
{
    ImGui::Text("Monster Spawn Trigger Editor");
    ImGui::Separator();

    m_pMouse = Mouse::GetInstance();
    ImGuiIO& io = ImGui::GetIO();

    ObjCreator();
    ObjEditor();

    return 0;
}

void Trigger_Controller_UI::ObjCreator()
{
    if (ImGui::CollapsingHeader("Trigger Creator", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputFloat3("Spawn Position", ColPos);

        if (g_pSelectedObject != nullptr)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));

            if (ImGui::Button("Get Selected Object Position", ImVec2(-1, 25)))
            {
                _float3 fTargetPos;
                XMStoreFloat3(&fTargetPos, g_pSelectedObject->Get_Position());

                ColPos[0] = fTargetPos.x;
                ColPos[1] = fTargetPos.y;
                ColPos[2] = fTargetPos.z;
            }
            ImGui::PopStyleColor();
        }

        ImGui::Spacing();
        ImGui::InputFloat3("Spawn Scale", ColScale);
        ImGui::Spacing();

        if (ImGui::Button("Create Spawn Trigger", ImVec2(-1, 30)))
        {
            Monster_EventShape::MonsterEventShapeDesc desc;
            desc.pOwner = nullptr;
            desc.vOffSet = _float3(ColPos[0], ColPos[1], ColPos[2]);
            desc.Extents = _float3(ColScale[0] * 0.5f, ColScale[1] * 0.5f, ColScale[2] * 0.5f);
            desc.eColliderType = COLLIDER::AABB;

            m_pGameInstance->Add_GameObject_To_Layer(_uint(LEVEL::TOOL), L"Prototype_GameObject_Monster_EventShape",
                _uint(LEVEL::TOOL), L"Layer_Trigger", nullptr, &desc);
        }
        ImGui::Separator();
    }
}

void Trigger_Controller_UI::ObjEditor()
{
    if (ImGui::CollapsingHeader("Trigger Editor", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static Monster_EventShape* s_pEditingTrigger = nullptr;
        static int s_iPickingSpawnIndex = -1;

        if (s_pEditingTrigger && s_pEditingTrigger->Is_Dead())
        {
            s_pEditingTrigger = nullptr;
            s_iPickingSpawnIndex = -1;
        }

        Monster_EventShape* pPickedTrigger = dynamic_cast<Monster_EventShape*>(g_pSelectedObject);
        if (pPickedTrigger)
        {
            if (s_pEditingTrigger != pPickedTrigger)
                s_iPickingSpawnIndex = -1;

            s_pEditingTrigger = pPickedTrigger;
        }

        if (s_pEditingTrigger == nullptr)
        {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "No Trigger Selected.");
            return;
        }

        ImGui::PushID(s_pEditingTrigger);

        ImGui::Text("1. Trigger Box Settings");

        static GameObject* pPrevSelectedObject = nullptr;
        if (pPrevSelectedObject != s_pEditingTrigger)
        {
            XMStoreFloat3(&m_fPos, s_pEditingTrigger->Get_Position());
            m_fScale = s_pEditingTrigger->Get_Scale();
            m_fmulScale = 1.0f;
            m_fBaseScale = s_pEditingTrigger->Get_Scale();
            pPrevSelectedObject = s_pEditingTrigger;
        }

        if (ImGui::DragFloat3("Box Position", (float*)&m_fPos, 0.1f)) {
            _float4 fPosWithW = { m_fPos.x, m_fPos.y, m_fPos.z, 1.f };
            s_pEditingTrigger->Set_State(DIRECTION::POSITION, XMLoadFloat4(&fPosWithW));
        }

        bool bScaleChanged = false;
        if (ImGui::DragFloat3("Box Scale", (float*)&m_fScale, 0.05f, 0.01f, 100.f)) bScaleChanged = true;
        if (ImGui::DragFloat("Multiply Scale", &m_fmulScale, 0.01f, 0.01f, 10.f)) {
            m_fScale.x = m_fBaseScale.x * m_fmulScale;
            m_fScale.y = m_fBaseScale.y * m_fmulScale;
            m_fScale.z = m_fBaseScale.z * m_fmulScale;
            bScaleChanged = true;
        }

        if (bScaleChanged) s_pEditingTrigger->Set_Scale(m_fScale.x, m_fScale.y, m_fScale.z);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "2. Monster Spawn Settings");

        auto& spawnList = s_pEditingTrigger->Get_SpawnList();

        if (s_iPickingSpawnIndex >= spawnList.size())
        {
            s_iPickingSpawnIndex = -1;
        }

        if (ImGui::Button("Add Spawn Point", ImVec2(-1, 30)))
        {
            Monster_EventShape::MonsterSpawnInfo newInfo = {};
            newInfo.iMonsterID = 0;
            XMStoreFloat3(&newInfo.vPosition, s_pEditingTrigger->Get_Position());
            newInfo.vRotation = _float3(0.f, 0.f, 0.f);

            s_pEditingTrigger->Add_SpawnInfo(newInfo);
        }

        ImGui::Spacing();

        for (int i = 0; i < spawnList.size(); ++i)
        {
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

            if (ImGui::CollapsingHeader(("Spawn Target [" + to_string(i) + "]").c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                const char* monsterNames[] = {
                    "SLIME", "SLAVE_DEVIL", "SLAVE_VAMPIRE", "OLIVER",
                    "WOLFGHOST", "GIANT_VAMPIRE", "MONKEY_DEVIL"
                };

                int monsterIDs[] = {
                    (int)Engine::POOL_ID::MONSTER_SLIME,
                    (int)Engine::POOL_ID::MONSTER_SLAVE_DEVIL,
                    (int)Engine::POOL_ID::MONSTER_SLAVE_VAMPIRE,
                    (int)Engine::POOL_ID::MONSTER_OLIVER,
                    (int)Engine::POOL_ID::MONSTER_WOLFGHOST,
                    (int)Engine::POOL_ID::MONSTER_GIANT_VAMPIRE,
                    (int)Engine::POOL_ID::MONSTER_MONKEY_DEVIL
                };

                int currentIndex = 0;
                int numMonsters = IM_ARRAYSIZE(monsterIDs);

                for (int j = 0; j < numMonsters; ++j)
                {
                    if (spawnList[i].iMonsterID == monsterIDs[j])
                    {
                        currentIndex = j;
                        break;
                    }
                }

                if (ImGui::Combo("Monster Type", &currentIndex, monsterNames, IM_ARRAYSIZE(monsterNames)))
                {
                    spawnList[i].iMonsterID = monsterIDs[currentIndex];
                }

                if (s_iPickingSpawnIndex == i)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                    if (ImGui::Button("Cancel Map Picking", ImVec2(-1, 25)))
                    {
                        s_iPickingSpawnIndex = -1; // 모드 취소
                    }
                    ImGui::PopStyleColor();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Click on the map surface!");
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.8f, 1.0f));
                    if (ImGui::Button("Pick Position from Map", ImVec2(-1, 25)))
                    {
                        s_iPickingSpawnIndex = i;
                    }
                    ImGui::PopStyleColor();
                }

                ImGui::DragFloat3("Position", (float*)&spawnList[i].vPosition, 0.1f);
                ImGui::DragFloat3("Rotation", (float*)&spawnList[i].vRotation, 1.0f, 0.0f, 360.0f);

                if (ImGui::Button("Remove This Spawn", ImVec2(150, 20)))
                {
                    spawnList.erase(spawnList.begin() + i);
                    ImGui::PopStyleColor();
                    ImGui::PopID();
                    break;
                }
            }
            ImGui::PopStyleColor();
            ImGui::PopID();
        }

        if (s_iPickingSpawnIndex != -1)
        {
            if (!ImGui::GetIO().WantCaptureMouse && m_pGameInstance->MouseDown(MOUSEKEYSTATE::LB))
            {
                _float4 vPickPos;
                if (m_pGameInstance->PickingObject(&vPickPos) && vPickPos.w != 0.f)
                {
                    spawnList[s_iPickingSpawnIndex].vPosition = _float3(vPickPos.x, vPickPos.y, vPickPos.z);

                    spawnList[s_iPickingSpawnIndex].vPosition.y += 2.f; 

                    s_iPickingSpawnIndex = -1;
                }
            }
            else if (!ImGui::GetIO().WantCaptureMouse && m_pGameInstance->MouseDown(MOUSEKEYSTATE::RB))
            {
                s_iPickingSpawnIndex = -1;
            }
        }

        ImGui::PopID();
    }
}

Trigger_Controller_UI* Trigger_Controller_UI::Create(ID3D11Device* pD, ID3D11DeviceContext* pC, void* pArg)
{
    Trigger_Controller_UI* pIns = new Trigger_Controller_UI(pD, pC);
    if (FAILED(pIns->Initialize(pArg)))
    {
        MSG_BOX("Trigger_Controller_UI 생성 실패"); Safe_Release(pIns);
    }
    return pIns;
}

void Trigger_Controller_UI::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}