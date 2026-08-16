#include "Client_Define.h"
#include "IMGUI_MonsterEditor.h"
#include "GameInstance.h"
#include "Monster.h"


Client::IMGUI_MonsterEditor::IMGUI_MonsterEditor()
{
}

Client::IMGUI_MonsterEditor::IMGUI_MonsterEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :ImguiWindow(pDevice,pContext)
{
    m_pGameInstance = GameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);

}

Client::IMGUI_MonsterEditor::~IMGUI_MonsterEditor()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::IMGUI_MonsterEditor::Initialize(void* arg)
{
    m_WindowTitle = "MonsterEditor";

    ImGui::SetNextWindowSize(ImVec2(450.0f, 600.0f), ImGuiCond_FirstUseEver);

    // 오브젝트 교체 이벤트 구독
    m_pGameInstance->Subscribe<IMGUI_EVENT>([this](const IMGUI_EVENT& e) {
        if (e.eType == IMGUI_EVENT_TYPE::CHANGE_OBJ) {
            m_pTargetMonster = DCAST(Monster*)(e.pGameObject);
        }
        });



    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/


_uint Client::IMGUI_MonsterEditor::Update_Contents(_float fTimeDelta)
{
    if (m_pTargetMonster == nullptr) {
        ImGui::Text("Please Select a Monster in Scene");
        return 0;
    }

    ImGui::Text("Target: %ls", m_pTargetMonster->Get_Name().c_str());
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Movement Stats", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::DragFloat("Walk Speed", &m_pTargetMonster->m_fWalkSpeed, 0.1f, 0.f, 20.f))
            m_pTargetMonster->Set_Ranges_Square();

        if(ImGui::DragFloat("Run Speed", &m_pTargetMonster->m_fRunSpeed, 0.1f, 0.f, 50.f))
            m_pTargetMonster->Set_Ranges_Square();

        if (ImGui::DragFloat("Rotation Speed", &m_pTargetMonster->m_fChaseRotationSpeed, 0.1f, 0.f, 10.f))
            m_pTargetMonster->Set_Ranges_Square();
    }

    if (ImGui::CollapsingHeader("Detection & Range", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::DragFloat("Detect Range", &m_pTargetMonster->m_fDetectRange, 0.5f, 0.f, 100.f))
            m_pTargetMonster->Set_Ranges_Square();


        if (ImGui::DragFloat("Chase Range", &m_pTargetMonster->m_fChaseRange, 0.5f, 0.f, 100.f))
            m_pTargetMonster->Set_Ranges_Square();

        if (ImGui::DragFloat("Melee Attack Range", &m_pTargetMonster->m_fAttackMeleeRange, 0.1f, 0.f, 20.f))
        {
            m_pTargetMonster->m_fAttackMeleeRangeSq = m_pTargetMonster->m_fAttackMeleeRange * m_pTargetMonster->m_fAttackMeleeRange;
            m_pTargetMonster->Set_Ranges_Square();
        }

        if (ImGui::DragFloat("Far Attack Range", &m_pTargetMonster->m_fAttackFarRange, 0.1f, 0.f, 50.f))
        {
            m_pTargetMonster->m_fAttackFarRangeSq = m_pTargetMonster->m_fAttackFarRange * m_pTargetMonster->m_fAttackFarRange;
            m_pTargetMonster->Set_Ranges_Square();
        }
    }

    if (ImGui::CollapsingHeader("Combat Stats"))
    {
        ImGui::DragFloat("Max HP", &m_pTargetMonster->m_fMaxHp, 10.f, 1.f, 10000.f);
        ImGui::ProgressBar(m_pTargetMonster->m_fCurrentHp / m_pTargetMonster->m_fMaxHp, ImVec2(-1, 0));
        ImGui::DragFloat("Current HP", &m_pTargetMonster->m_fCurrentHp, 5.f, 0.f, m_pTargetMonster->m_fMaxHp);
        ImGui::DragFloat("Grit (SuperArmor)", &m_pTargetMonster->m_fCurrentGrit, 0.1f, 0.f, 100.f);
    }

    return 0;
}


//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
IMGUI_MonsterEditor* Client::IMGUI_MonsterEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* arg)
{
    IMGUI_MonsterEditor* pInstance = new IMGUI_MonsterEditor(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(arg), L"IMGUI_MonsterEditor 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/
//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::IMGUI_MonsterEditor::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/