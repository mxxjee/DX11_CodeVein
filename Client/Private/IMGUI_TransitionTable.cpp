#include "Client_Define.h"
#include "IMGUI_TransitionTable.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Monster.h"
#include "MState_TransitionTable.h"

// 상태 이름 테이블
static const char* s_StateNames[] =
{
    "MS_IDLE", "MS_PATROL", "MS_PATROL_TURN", "MS_AWAKE_IDLE","MS_AWAKE", "MS_FALLBACK",
    "MS_BATTLE", "MS_WALK", "MS_RUN", "MS_HIT", "MS_TURN",
    "MS_ATTACKMELEE0", "MS_ATTACKMELEE1", "MS_ATTACKMELEE2", "MS_ATTACKMELEE3", "MS_ATTACKMELEE4",
    "MS_ATTACKFAR0", "MS_ATTACKFAR1", "MS_ATTACKFAR2", "MS_ATTACKFAR3", "MS_ATTACKFAR4",
    "MS_ATTACKSPECIAL0", "MS_ATTACKSPECIAL1", "MS_ATTACKSPECIAL2", "MS_ATTACKSPECIAL3", "MS_ATTACKSPECIAL4",
    "MS_EVADE","MS_STUN", "MS_CINEMATIC", "MS_REPEL", "MS_DOWN", "MS_PHASECHANGE",
    "MS_DEAD"
};

static const char* s_CondNames[] =
{
    "DIST_LESS_THAN", "DIST_GREATER_THAN", "HP_LESS_THAN",
    "RANDOM_CHANCE", "ANIM_FINISHED", "NOT_IN_FOV",
    "OUT_OF_CHASE_RANGE", "CAN_ATTACK",
    "IN_DETECT_RANGE", "IN_CHASE_RANGE", "IN_WALK_RANGE",
    "IN_MELEE_RANGE", "IN_FAR_RANGE", "NOT_SAME_PATTERN", "IS_FIRST_AWARE", "IS_HITTYPE_EQUAL",
    "DO_COMBO_ATTACK", "NOT_YET_USED",
};



//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::IMGUI_TransitionTable::IMGUI_TransitionTable()
{
}

Client::IMGUI_TransitionTable::IMGUI_TransitionTable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : ImguiWindow(pDevice, pContext)
{
    m_pGameInstance = GameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);
}

Client::IMGUI_TransitionTable::~IMGUI_TransitionTable()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::IMGUI_TransitionTable::Initialize(void* arg)
{
    m_WindowTitle = "Transition Table";

    ImGui::SetNextWindowSize(ImVec2(450.0f, 600.0f), ImGuiCond_FirstUseEver);

    // 오브젝트 교체 이벤트 구독
    m_pGameInstance->Subscribe<IMGUI_EVENT>([this](const IMGUI_EVENT& e) {
        if (e.eType == IMGUI_EVENT_TYPE::CHANGE_OBJ)
        {
            Monster* pMonster = DCAST(Monster*)(e.pGameObject);
            if (pMonster != nullptr)
            {
                m_pTargetMonster = pMonster;
                m_pTable = pMonster->Get_TransitionTable();
                m_iSelectedRule = -1;
                m_bOpen = true;
            }
        }
        });

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 메인 그리기 ////////////////////////////////////////////////////////
_uint Client::IMGUI_TransitionTable::Update_Contents(_float fTimeDelta)
{
    if (m_pTable == nullptr || m_pTargetMonster == nullptr)
    {
        ImGui::Text("No monster selected");
        return 0;
    }

    Draw_StateSelector();
    ImGui::Separator();
    Draw_RuleList();
    ImGui::Separator();
    Draw_RuleEditor();
    ImGui::Separator();
    Draw_Buttons();

    return 0;
}
/******************************************************* 메인 그리기 *******************************************************/



//////////////////////////////////////////////////////// 상태 선택 ////////////////////////////////////////////////////////
void Client::IMGUI_TransitionTable::Draw_StateSelector()
{
    ImGui::Text("From State:");
    if (ImGui::BeginCombo("##FromState", Get_StateName(m_iSelectedFrom)))
    {
        for (_uint i = 0; i < Monster::MS_END; ++i)
        {
            _bool bSelected = (m_iSelectedFrom == i);
            if (ImGui::Selectable(s_StateNames[i], bSelected))
            {
                m_iSelectedFrom = i;
                m_iSelectedRule = -1;
            }
            if (bSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}
/******************************************************* 상태 선택 *******************************************************/



//////////////////////////////////////////////////////// 룰 목록 ////////////////////////////////////////////////////////
void Client::IMGUI_TransitionTable::Draw_RuleList()
{
    const vector<TRANSITION_RULE>* pRules = m_pTable->Find_Rules(m_iSelectedFrom);

    ImGui::Text("Rules:");

    if (pRules == nullptr || pRules->empty())
    {
        ImGui::TextDisabled("No rules for this state");
    }
    else
    {
        if (ImGui::BeginChild("##RuleList", ImVec2(0, 150), ImGuiChildFlags_Borders))
        {
            for (_uint i = 0; i < pRules->size(); ++i)
            {
                const auto& rule = (*pRules)[i];
                // 우선순위 + 타겟 상태로 표시
                string label = "[" + to_string(rule.iPriority) + "] -> " +
                    string(Get_StateName(rule.iTargetState)) +
                    " (" + to_string(rule.vecConditions.size()) + " conds)##" + to_string(i);

                if (ImGui::Selectable(label.c_str(), m_iSelectedRule == (_int)i))
                    m_iSelectedRule = i;
            }
        }
        ImGui::EndChild();
    }

    // 룰 추가/삭제 버튼
    if (ImGui::Button("Add Rule", ImVec2(80, 20)))
    {
        TRANSITION_RULE newRule = {};
        newRule.iPriority = pRules ? (_uint)pRules->size() : 0;
        newRule.iTargetState = Monster::MS_BATTLE;
        m_pTable->Add_Rule(m_iSelectedFrom, newRule);
    }

#ifdef _DEBUG
    ImGui::SameLine();

    if (ImGui::Button("Remove Rule", ImVec2(100, 20)))
    {
        if (m_iSelectedRule >= 0)
        {
            m_pTable->Remove_Rule(m_iSelectedFrom, m_iSelectedRule);
            m_iSelectedRule = -1;
        }
    }
#endif // _DEBUG


}
/******************************************************* 룰 목록 *******************************************************/



//////////////////////////////////////////////////////// 룰 편집기 ////////////////////////////////////////////////////////
void Client::IMGUI_TransitionTable::Draw_RuleEditor()
{
    if (m_iSelectedRule < 0)
    {
        ImGui::TextDisabled("Select a rule to edit");
        return;
    }

    // 직접 수정 가능하도록 non-const 접근
    vector<TRANSITION_RULE>* pRules = m_pTable->Get_Rules_Mutable(m_iSelectedFrom);
    if (pRules == nullptr || m_iSelectedRule >= (_int)pRules->size())
        return;

    TRANSITION_RULE& rule = (*pRules)[m_iSelectedRule];

    ImGui::Text("Edit Rule [%d]", m_iSelectedRule);

    // 타겟 상태 선택
    ImGui::Text("Target:");
    ImGui::SameLine(80);
    if (ImGui::BeginCombo("##Target", Get_StateName(rule.iTargetState)))
    {
        for (_uint i = 0; i < Monster::MS_END; ++i)
        {
            if (ImGui::Selectable(s_StateNames[i], rule.iTargetState == i))
                rule.iTargetState = i;
        }
        ImGui::EndCombo();
    }

    // 우선순위
    _int iPriority = (_int)rule.iPriority;
    ImGui::Text("Priority:");
    ImGui::SameLine(80);
    if (ImGui::InputInt("##Priority", &iPriority))
    {
        if (iPriority >= 0)
        {
            rule.iPriority = (_uint)iPriority;
            // 우선순위 변경 시 재정렬
            m_pTable->Sort_Rules_Public(m_iSelectedFrom);
            m_iSelectedRule = -1;
        }
    }

    ImGui::Separator();
    ImGui::Text("Conditions:");

    // 조건 목록 편집
    for (_uint i = 0; i < rule.vecConditions.size(); ++i)
    {
        ImGui::PushID(i);
        Draw_ConditionEditor(rule.vecConditions[i], i);

        ImGui::SameLine();
        if (ImGui::Button("X", ImVec2(20, 20)))
        {
            rule.vecConditions.erase(rule.vecConditions.begin() + i);
            ImGui::PopID();
            break;
        }
        ImGui::PopID();
    }

    if (ImGui::Button("Add Condition", ImVec2(120, 20)))
    {
        TRANSITION_COND newCond = {};
        newCond.eCondition = TRANSITION_CONDITION::DIST_LESS_THAN;
        newCond.fValue = 0.f;
        newCond.fValueSq = 0.f;
        rule.vecConditions.push_back(newCond);
    }
}
/******************************************************* 룰 편집기 *******************************************************/



//////////////////////////////////////////////////////// 조건 편집기 ////////////////////////////////////////////////////////
void Client::IMGUI_TransitionTable::Draw_ConditionEditor(TRANSITION_COND& _cond, _uint _index)
{
    // 조건 타입 콤보
    _int iCond = (_int)_cond.eCondition;
    ImGui::SetNextItemWidth(180);
    if (ImGui::Combo(("##Cond" + to_string(_index)).c_str(), &iCond, s_CondNames, IM_ARRAYSIZE(s_CondNames)))
    {
        _cond.eCondition = CAST(TRANSITION_CONDITION)(iCond);
    }

    // 값이 필요한 조건만 value 입력 표시
    if (_cond.eCondition == TRANSITION_CONDITION::DIST_LESS_THAN ||
        _cond.eCondition == TRANSITION_CONDITION::DIST_GREATER_THAN ||
        _cond.eCondition == TRANSITION_CONDITION::HP_LESS_THAN ||
        _cond.eCondition == TRANSITION_CONDITION::RANDOM_CHANCE||
        _cond.eCondition == TRANSITION_CONDITION::IS_HITTYPE_EQUAL)
    {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        if (ImGui::DragFloat(("##Val" + to_string(_index)).c_str(), &_cond.fValue, 0.1f, 0.f, 100.f, "%.1f"))
        {
            // 거리 조건은 제곱 캐싱 갱신
            if (_cond.eCondition == TRANSITION_CONDITION::DIST_LESS_THAN ||
                _cond.eCondition == TRANSITION_CONDITION::DIST_GREATER_THAN)
            {
                _cond.fValueSq = _cond.fValue * _cond.fValue;
            }
            else
            {
                _cond.fValueSq = _cond.fValue;
            }
        }
    }
}
/******************************************************* 조건 편집기 *******************************************************/



//////////////////////////////////////////////////////// 버튼 ////////////////////////////////////////////////////////
void Client::IMGUI_TransitionTable::Draw_Buttons()
{
#ifdef _DEBUG
    static char monsterName[128] = "WolfGhost";
    static const char* basePath = "../../DataFiles/Monster_StateTable/";

    ImGui::InputText("##MonsterName", monsterName, IM_ARRAYSIZE(monsterName));

    string fullPath = string(basePath) + string(monsterName) + ".jsonc";

    if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f - 4.f, 24.f)))
    {
        _wstring wstrPath(fullPath.begin(), fullPath.end());
        m_pTable->Save(wstrPath);
        COUT("Transition Table Saved: " << fullPath);
    }

    ImGui::SameLine();

    if (ImGui::Button("Load", ImVec2(-1.f, 24.f)))
    {
        _wstring wstrPath(fullPath.begin(), fullPath.end());
        m_pTable->Reload(wstrPath);
        m_iSelectedRule = -1;
        COUT("Transition Table Loaded: " << fullPath);
    }
#endif
}
    /******************************************************* 버튼 *******************************************************/



//////////////////////////////////////////////////////// 이름 변환 ////////////////////////////////////////////////////////
const char* Client::IMGUI_TransitionTable::Get_StateName(_uint _iState) const
{
    if (_iState >= Monster::MS_END)
        return "UNKNOWN";

    return s_StateNames[_iState];
}

const char* Client::IMGUI_TransitionTable::Get_ConditionName(TRANSITION_CONDITION _eCond) const
{
    _uint index = _UINT(_eCond);
    if (index >= _UINT(TRANSITION_CONDITION::CONDITION_END))
        return "UNKNOWN";

    return s_CondNames[index];
}
/******************************************************* 이름 변환 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
_int Client::IMGUI_TransitionTable::Render(const _float fTimeDelta)
{
    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
IMGUI_TransitionTable* Client::IMGUI_TransitionTable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* arg)
{
    IMGUI_TransitionTable* pInstance = new IMGUI_TransitionTable(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(arg), L"IMGUI_TransitionTable 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::IMGUI_TransitionTable::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/