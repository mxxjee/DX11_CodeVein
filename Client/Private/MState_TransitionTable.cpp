#include "Client_Define.h"
#include "MState_TransitionTable.h"
#include "Monster.h"
#include <fstream>
#include <json.hpp>
#include <algorithm>

using json = nlohmann::json;

//////////////////////////////////////////////////////// 생성자 ////////////////////////////////////////////////////////
Client::MState_TransitionTable::MState_TransitionTable()
{
}
/******************************************************* 생성자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_TransitionTable::Initialize(const _wstring& _filePath)
{
    CHECK_FAILED(Load(_filePath), E_FAIL);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// JSON 로드 ////////////////////////////////////////////////////////
HRESULT Client::MState_TransitionTable::Load(const _wstring& _filePath)
{
    ifstream file(_filePath);

    if (!file.is_open())
    {
        MSG_ON((L"전이 테이블 " + _filePath + L"의 로드에 실패했습니다!").c_str(), L"전이 테이블 로드 실패");
        return E_FAIL;
    }

    json j;
    string strContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    j = json::parse(strContent, nullptr, true, true);  // 마지막 true = 주석 허용

    // string -> Monster::MONSTER_STATE 매핑 (로드 시점에만 사용)
    static const UMAP<string, _uint> stateMap =
    {
        {"MS_IDLE",         Monster::MS_IDLE},
        {"MS_PATROL",       Monster::MS_PATROL},
        {"MS_PATROL_TURN",  Monster::MS_PATROL_TURN},
        {"MS_AWAKE_IDLE",   Monster::MS_AWAKE_IDLE},
        {"MS_AWAKE",        Monster::MS_AWAKE},
        {"MS_FALLBACK",     Monster::MS_FALLBACK},
        {"MS_BATTLE",       Monster::MS_BATTLE},
        {"MS_ATTACKSPECIAL0", Monster::MS_ATTACKSPECIAL0},
        {"MS_ATTACKSPECIAL1", Monster::MS_ATTACKSPECIAL1},
        {"MS_ATTACKSPECIAL2", Monster::MS_ATTACKSPECIAL2},
        {"MS_ATTACKSPECIAL3", Monster::MS_ATTACKSPECIAL3},
        {"MS_ATTACKSPECIAL4", Monster::MS_ATTACKSPECIAL4},
        {"MS_ATTACKMELEE0", Monster::MS_ATTACKMELEE0},
        {"MS_ATTACKMELEE1", Monster::MS_ATTACKMELEE1},
        {"MS_ATTACKMELEE2", Monster::MS_ATTACKMELEE2},
        {"MS_ATTACKMELEE3", Monster::MS_ATTACKMELEE3},
        {"MS_ATTACKMELEE4", Monster::MS_ATTACKMELEE4},
        {"MS_ATTACKFAR0",   Monster::MS_ATTACKFAR0},
        {"MS_ATTACKFAR1",   Monster::MS_ATTACKFAR1},
        {"MS_ATTACKFAR2",   Monster::MS_ATTACKFAR2},
        {"MS_ATTACKFAR3",   Monster::MS_ATTACKFAR3},
        {"MS_ATTACKFAR4",   Monster::MS_ATTACKFAR4},
        {"MS_HIT",          Monster::MS_HIT},
        {"MS_TURN",         Monster::MS_TURN},
        {"MS_WALK",         Monster::MS_WALK},
        {"MS_RUN",          Monster::MS_RUN},
        {"MS_EVADE",        Monster::MS_EVADE},
        {"MS_STUN",         Monster::MS_STUN},
        {"MS_CINEMATIC",    Monster::MS_CINEMATIC},
        {"MS_REPEL",        Monster::MS_REPEL},
        {"MS_DOWN",         Monster::MS_DOWN},
        {"MS_PHASECHANGE",  Monster::MS_PHASECHANGE},
        {"MS_DEAD",         Monster::MS_DEAD},
        //{"MS_SIZING",       Monster::MS_SIZING},
    };

    // 컨티션 맵
    static const UMAP<string, TRANSITION_CONDITION> condMap =
    {
        {"DIST_LESS_THAN",      TRANSITION_CONDITION::DIST_LESS_THAN},
        {"DIST_GREATER_THAN",   TRANSITION_CONDITION::DIST_GREATER_THAN},
        {"HP_LESS_THAN",        TRANSITION_CONDITION::HP_LESS_THAN},
        {"RANDOM_CHANCE",       TRANSITION_CONDITION::RANDOM_CHANCE},
        {"ANIM_FINISHED",       TRANSITION_CONDITION::ANIM_FINISHED},
        {"NOT_IN_FOV",          TRANSITION_CONDITION::NOT_IN_FOV},
        {"OUT_OF_CHASE_RANGE",  TRANSITION_CONDITION::OUT_OF_CHASE_RANGE},
        {"CAN_ATTACK",          TRANSITION_CONDITION::CAN_ATTACK},
        {"IN_DETECT_RANGE",     TRANSITION_CONDITION::IN_DETECT_RANGE},
        {"IN_CHASE_RANGE",      TRANSITION_CONDITION::IN_CHASE_RANGE},
        {"IN_WALK_RANGE",       TRANSITION_CONDITION::IN_WALK_RANGE},
        {"IN_MELEE_RANGE",      TRANSITION_CONDITION::IN_MELEE_RANGE},
        {"IN_FAR_RANGE",        TRANSITION_CONDITION::IN_FAR_RANGE},
        {"NOT_SAME_PATTERN",    TRANSITION_CONDITION::NOT_SAME_PATTERN},
        {"IS_FIRST_AWARE",      TRANSITION_CONDITION::IS_FIRST_AWARE},
        {"IS_HITTYPE_EQUAL",    TRANSITION_CONDITION::IS_HITTYPE_EQUAL},
        {"DO_COMBO_ATTACK",     TRANSITION_CONDITION::DO_COMBO_ATTACK},
        {"NOT_YET_USED",        TRANSITION_CONDITION::NOT_YET_USED},
    };

    for (auto& transEntry : j["transitions"])
    {
        string strFrom = transEntry["from"].get<string>();
        auto itFrom = stateMap.find(strFrom);

        if (itFrom == stateMap.end())
        {
            COUT("전이 테이블이 [" << strFrom << "] 상태를 읽을 수 없습니다");
            continue;
        }

        _uint iFromState = itFrom->second;

        for (auto& ruleEntry : transEntry["rules"])
        {
            TRANSITION_RULE rule = {};

            // 복합 조건 파싱
            for (auto& condEntry : ruleEntry["conditions"])
            {
                string strCond = condEntry["type"].get<string>();

                auto itCond = condMap.find(strCond);
                if (itCond == condMap.end())
                {
                    COUT("전이 테이블이 [" << strCond << "] 조건을 읽을 수 없습니다");
                    continue;
                }

                TRANSITION_COND cond = {};
                cond.eCondition = itCond->second;
                cond.fValue = condEntry["value"].get<_float>();

                // 거리 조건은 제곱으로 캐싱
                if (cond.eCondition == TRANSITION_CONDITION::DIST_LESS_THAN ||
                    cond.eCondition == TRANSITION_CONDITION::DIST_GREATER_THAN)
                {
                    cond.fValueSq = cond.fValue * cond.fValue;
                }
                else
                {
                    cond.fValueSq = cond.fValue;
                }

                rule.vecConditions.push_back(cond);
            }

            string strTarget = ruleEntry["target"].get<string>();
            auto itTarget = stateMap.find(strTarget);

            if (itTarget == stateMap.end())
            {
                COUT("전이 테이블이 [" << strTarget << "] 타겟을 읽을 수 없습니다");
                continue;
            }

            rule.iTargetState = itTarget->second;
            rule.iPriority = ruleEntry["priority"].get<_uint>();

            m_arrTable[iFromState].push_back(rule);
        }

        Sort_Rules(iFromState);
    }

    return S_OK;
}
/******************************************************* JSON 로드 *******************************************************/



//////////////////////////////////////////////////////// 룰 조회 ////////////////////////////////////////////////////////
const vector<Client::TRANSITION_RULE>* Client::MState_TransitionTable::Find_Rules(_uint _iFromState) const
{
    if (_iFromState >= Monster::MS_END)
        return nullptr;

    if (m_arrTable[_iFromState].empty())
        return nullptr;

    return &m_arrTable[_iFromState];
}
/******************************************************* 룰 조회 *******************************************************/



//////////////////////////////////////////////////////// 수동 룰 추가 ////////////////////////////////////////////////////////
void Client::MState_TransitionTable::Add_Rule(_uint _iFromState, const TRANSITION_RULE& _rule)
{
    if (_iFromState >= Monster::MS_END)
        return;

    m_arrTable[_iFromState].push_back(_rule);
    Sort_Rules(_iFromState);
}
/******************************************************* 수동 룰 추가 *******************************************************/



//////////////////////////////////////////////////////// 클리어 ////////////////////////////////////////////////////////
void Client::MState_TransitionTable::Clear()
{
    for (_uint i = 0; i < Monster::MS_END; ++i)
        m_arrTable[i].clear();
}
/******************************************************* 클리어 *******************************************************/



//////////////////////////////////////////////////////// 핫 리로드(IMGUI용) ////////////////////////////////////////////////////////
HRESULT Client::MState_TransitionTable::Reload(const _wstring& _filePath)
{
    Clear();
    return Load(_filePath);
}

vector<Client::TRANSITION_RULE>* Client::MState_TransitionTable::Get_Rules_Mutable(_uint _iFromState)
{
    if (_iFromState >= Monster::MS_END)
        return nullptr;

    if (m_arrTable[_iFromState].empty())
        return nullptr;

    return &m_arrTable[_iFromState];
}

HRESULT Client::MState_TransitionTable::Save(const _wstring& _filePath) const
{
    // enum -> string 변환 테이블
    static const unordered_map<_uint, string> stateNameMap =
    {
        {Monster::MS_IDLE,          "MS_IDLE"},
        {Monster::MS_PATROL,        "MS_PATROL"},
        {Monster::MS_PATROL_TURN,   "MS_PATROL_TURN"},
        {Monster::MS_AWAKE_IDLE,    "MS_AWAKE_IDLE"},
        {Monster::MS_AWAKE,         "MS_AWAKE"},
        {Monster::MS_FALLBACK,      "MS_FALLBACK"},
        {Monster::MS_BATTLE,        "MS_BATTLE"},
        {Monster::MS_WALK,          "MS_WALK"},
        {Monster::MS_RUN,           "MS_RUN"},
        {Monster::MS_HIT,           "MS_HIT"},
        {Monster::MS_TURN,          "MS_TURN"},
        {Monster::MS_ATTACKSPECIAL0,  "MS_ATTACKSPECIAL0"},
        {Monster::MS_ATTACKSPECIAL1,  "MS_ATTACKSPECIAL1"},
        {Monster::MS_ATTACKSPECIAL2,  "MS_ATTACKSPECIAL2"},
        {Monster::MS_ATTACKSPECIAL3,  "MS_ATTACKSPECIAL3"},
        {Monster::MS_ATTACKSPECIAL4,  "MS_ATTACKSPECIAL4"},
        {Monster::MS_ATTACKMELEE0,  "MS_ATTACKMELEE0"},
        {Monster::MS_ATTACKMELEE1,  "MS_ATTACKMELEE1"},
        {Monster::MS_ATTACKMELEE2,  "MS_ATTACKMELEE2"},
        {Monster::MS_ATTACKMELEE3,  "MS_ATTACKMELEE3"},
        {Monster::MS_ATTACKMELEE4,  "MS_ATTACKMELEE4"},
        {Monster::MS_ATTACKFAR0,    "MS_ATTACKFAR0"},
        {Monster::MS_ATTACKFAR1,    "MS_ATTACKFAR1"},
        {Monster::MS_ATTACKFAR2,    "MS_ATTACKFAR2"},
        {Monster::MS_ATTACKFAR3,    "MS_ATTACKFAR3"},
        {Monster::MS_ATTACKFAR4,    "MS_ATTACKFAR4"},
        {Monster::MS_STUN,          "MS_STUN"},
        {Monster::MS_CINEMATIC,     "MS_CINEMATIC"},
        {Monster::MS_REPEL,         "MS_REPEL"},
        {Monster::MS_DOWN,          "MS_DOWN"},
        {Monster::MS_PHASECHANGE,   "MS_PHASECHANGE"},
        {Monster::MS_DEAD,          "MS_DEAD"},
    };

    static const unordered_map<TRANSITION_CONDITION, string> condNameMap =
    {
        {TRANSITION_CONDITION::DIST_LESS_THAN,      "DIST_LESS_THAN"},
        {TRANSITION_CONDITION::DIST_GREATER_THAN,   "DIST_GREATER_THAN"},
        {TRANSITION_CONDITION::HP_LESS_THAN,        "HP_LESS_THAN"},
        {TRANSITION_CONDITION::RANDOM_CHANCE,       "RANDOM_CHANCE"},
        {TRANSITION_CONDITION::ANIM_FINISHED,       "ANIM_FINISHED"},
        {TRANSITION_CONDITION::NOT_IN_FOV,          "NOT_IN_FOV"},
        {TRANSITION_CONDITION::OUT_OF_CHASE_RANGE,  "OUT_OF_CHASE_RANGE"},
        {TRANSITION_CONDITION::CAN_ATTACK,          "CAN_ATTACK"},
        {TRANSITION_CONDITION::IN_DETECT_RANGE,     "IN_DETECT_RANGE"},
        {TRANSITION_CONDITION::IN_CHASE_RANGE,      "IN_CHASE_RANGE"},
        {TRANSITION_CONDITION::IN_WALK_RANGE,       "IN_WALK_RANGE"},
        {TRANSITION_CONDITION::IN_MELEE_RANGE,      "IN_MELEE_RANGE"},
        {TRANSITION_CONDITION::IN_FAR_RANGE,        "IN_FAR_RANGE"},
        {TRANSITION_CONDITION::NOT_SAME_PATTERN,    "NOT_SAME_PATTERN"},
        {TRANSITION_CONDITION::DO_COMBO_ATTACK,     "DO_COMBO_ATTACK"},
        {TRANSITION_CONDITION::NOT_YET_USED,        "NOT_YET_USED"},
    };

    json j;
    j["transitions"] = json::array();

    for (_uint i = 0; i < Monster::MS_END; ++i)
    {
        if (m_arrTable[i].empty())
            continue;

        auto itFrom = stateNameMap.find(i);
        if (itFrom == stateNameMap.end())
            continue;

        json transEntry;
        transEntry["from"] = itFrom->second;
        transEntry["rules"] = json::array();

        for (const auto& rule : m_arrTable[i])
        {
            json ruleEntry;
            ruleEntry["conditions"] = json::array();

            for (const auto& cond : rule.vecConditions)
            {
                auto itCond = condNameMap.find(cond.eCondition);
                if (itCond == condNameMap.end())
                    continue;

                json condEntry;
                condEntry["type"] = itCond->second;
                condEntry["value"] = cond.fValue;

                ruleEntry["conditions"].push_back(condEntry);
            }

            auto itTarget = stateNameMap.find(rule.iTargetState);
            if (itTarget == stateNameMap.end())
                continue;

            ruleEntry["target"] = itTarget->second;
            ruleEntry["priority"] = rule.iPriority;

            transEntry["rules"].push_back(ruleEntry);
        }

        j["transitions"].push_back(transEntry);
    }

    ofstream file(_filePath);
    if (!file.is_open())
    {
        WCOUT("전이 테이블 저장에 실패했습니다! : " << _filePath);
        return E_FAIL;
    }

    // 들여쓰기 4칸으로 저장
    file << j.dump(4);

    return S_OK;
}

void Client::MState_TransitionTable::Remove_Rule(_uint _iFromState, _uint _iRuleIndex)
{
    if (_iFromState >= Monster::MS_END)
        return;

    auto& vec = m_arrTable[_iFromState];
    if (_iRuleIndex >= vec.size())
        return;

    vec.erase(vec.begin() + _iRuleIndex);
}
/******************************************************* 핫 리로드(IMGUI용) *******************************************************/



//////////////////////////////////////////////////////// 정렬 ////////////////////////////////////////////////////////
void Client::MState_TransitionTable::Sort_Rules(_uint _iFromState)
{
    if (_iFromState >= Monster::MS_END)
        return;

    std::sort(m_arrTable[_iFromState].begin(), m_arrTable[_iFromState].end(),
        [](const TRANSITION_RULE& _a, const TRANSITION_RULE& _b)
        {
            return _a.iPriority < _b.iPriority;
        });
}
/******************************************************* 정렬 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_TransitionTable* Client::MState_TransitionTable::Create(const _wstring& _filePath)
{
    MState_TransitionTable* pInstance = new MState_TransitionTable();

    MSG_FAIL(pInstance->Initialize(_filePath), L"MState_TransitionTable 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_TransitionTable::Free()
{
    __super::Free();

    Clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/
