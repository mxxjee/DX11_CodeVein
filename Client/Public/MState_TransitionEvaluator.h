#pragma once
#include "Client_Define.h"
#include "MState_TransitionTypes.h"

NS_BEGIN(Client)

class Monster;

class MState_TransitionEvaluator
{
public:
    // 단일 조건 평가
    static _bool Evaluate_Condition(const TRANSITION_COND& _cond, const TRANSITION_RULE& _rule, Monster* _monster);

    // 룰 하나의 모든 조건을 AND로 평가
    static _bool    Evaluate_Rule(const TRANSITION_RULE& _rule, Monster* _monster);

    // 룰 목록에서 첫 번째 통과하는 룰의 타겟 상태 반환
    static _bool    Find_NextState(const vector<TRANSITION_RULE>& _rules, Monster* _monster, _uint& _outState);

private:
    MState_TransitionEvaluator() = delete;
    ~MState_TransitionEvaluator() = delete;
};

NS_END