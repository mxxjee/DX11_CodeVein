#include "Client_Define.h"
#include "MState_TransitionEvaluator.h"
#include "Monster.h"

_bool Client::MState_TransitionEvaluator::Evaluate_Condition(const TRANSITION_COND& _cond, const TRANSITION_RULE& _rule, Monster* _monster)
{
    CHECK_NULL_RESULT(_monster, false);

    switch (_cond.eCondition)
    {
    case TRANSITION_CONDITION::DIST_LESS_THAN:
        return _monster->Get_DistanceSq() < (_cond.fValueSq);

    case TRANSITION_CONDITION::DIST_GREATER_THAN:
        return _monster->Get_DistanceSq() > (_cond.fValueSq);

    case TRANSITION_CONDITION::HP_LESS_THAN:
        return _monster->Get_HPRatio() < _cond.fValue;

    case TRANSITION_CONDITION::IN_DETECT_RANGE:
        return _monster->Get_DistanceSq() <= _monster->Get_DetectRangeSq();

    case TRANSITION_CONDITION::IN_CHASE_RANGE:
        return _monster->Get_DistanceSq() <= _monster->Get_ChaseRangeSq();

    case TRANSITION_CONDITION::IN_WALK_RANGE:
        return _monster->Get_DistanceSq() <= _monster->Get_WalkRangeSq();

    case TRANSITION_CONDITION::IN_MELEE_RANGE:
        return _monster->Get_DistanceSq() <= _monster->Get_AttackRange_MeleeSq();

    case TRANSITION_CONDITION::IN_FAR_RANGE:
        return _monster->Get_DistanceSq() <= _monster->Get_AttackRange_FarSq();

    case TRANSITION_CONDITION::CAN_ATTACK:
        return _monster->Can_Attack();

    case TRANSITION_CONDITION::RANDOM_CHANCE:
        return _monster->Get_RandomValue() <= _cond.fValue;

    case TRANSITION_CONDITION::ANIM_FINISHED:
        return _monster->Is_Animation_Finished();

    case TRANSITION_CONDITION::NOT_IN_FOV:
        return !_monster->Detect_With_FOV();

    case TRANSITION_CONDITION::OUT_OF_CHASE_RANGE:
        return !_monster->Chase_Player();

    case TRANSITION_CONDITION::NOT_SAME_PATTERN:
        return _monster->Get_CurrentPattern() != _rule.iTargetState;

    case TRANSITION_CONDITION::IS_FIRST_AWARE:
        return _monster->Is_FirstAware()==_cond.fValue;

    case TRANSITION_CONDITION::IS_HITTYPE_EQUAL:
        return _UINT(_monster->Get_HitLevel()) == _cond.fValue;

    case TRANSITION_CONDITION::DO_COMBO_ATTACK:
        return _monster->Check_ComboAttack() == true;

    case TRANSITION_CONDITION::NOT_YET_USED:
        return !_monster->Is_StateUsed(_rule.iTargetState);
    default:
        return false;
    }
}

_bool Client::MState_TransitionEvaluator::Evaluate_Rule(const TRANSITION_RULE& _rule, Monster* _monster)
{
    // 조건이 비어있으면 항상 통과
    if (_rule.vecConditions.empty())
        return true;

    for (const auto& cond : _rule.vecConditions)
    {
        if (!Evaluate_Condition(cond, _rule, _monster))
            return false;
    }

    return true;
}

_bool Client::MState_TransitionEvaluator::Find_NextState(const vector<TRANSITION_RULE>& _rules, Monster* _monster, _uint& _outState)
{
    for (const auto& rule : _rules)
    {
        if (Evaluate_Rule(rule, _monster))
        {
            _outState = rule.iTargetState;
            return true;
        }
    }

    return false;
}