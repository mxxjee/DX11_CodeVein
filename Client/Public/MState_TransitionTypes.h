#pragma once
#include "Client_Define.h"

NS_BEGIN(Client)

// 전이 조건의 종류
// 수정할때 IMGUI도 수정 해야됨
enum class TRANSITION_CONDITION : _uint
{
    DIST_LESS_THAN,         // 플레이어와의 거리가 조건보다 적게 있는지
    DIST_GREATER_THAN,      // 플레이어와의 거리가 조건보다 멀리 있는지
    HP_LESS_THAN,           // HP 비율 < fValue
    RANDOM_CHANCE,          // 확률 (0~100, 매 프레임 판정)
    ANIM_FINISHED,          // 현재 애니메이션 종료
    NOT_IN_FOV,             // 시야각 밖
    OUT_OF_CHASE_RANGE,     // 추격 범위 밖
    CAN_ATTACK,             // 공격 가능한지

    IN_DETECT_RANGE,        // 감지 범위 안에 있는지
    IN_CHASE_RANGE,         // 추격 범위 안에 있는지
    IN_WALK_RANGE,          // 어디서부터 걸을지
    IN_MELEE_RANGE,         // 근접 공격 범위 안에 있는지
    IN_FAR_RANGE,           // 원거리 공격 범위 안에 있는지
    NOT_SAME_PATTERN,        // 사용할 패턴과 이 전 패턴이 같은지

    IS_FIRST_AWARE,         //플레이어를 감지한게 이번 처음인지
    IS_HITTYPE_EQUAL,       //HIT타입이 같은지 판단(enum순서)
    DO_COMBO_ATTACK,        // 연계 공격 할건지
    NOT_YET_USED,           // 사용했던 상태인지

    CONDITION_END
};

// 전이 규칙 하나 (종류 + 수치)
struct TRANSITION_COND
{
    TRANSITION_CONDITION    eCondition = {};
    _float                  fValue = {};    // 거리, 체력퍼센트, 확률 등등 다 가능
    _float                  fValueSq = {};  // 제곱 캐싱
};

// 전이 규칙 여러개
struct TRANSITION_RULE
{
    vector<TRANSITION_COND>     vecConditions;      // 전부 통과해야 전이
    _uint                       iTargetState = {};
    _uint                       iPriority = {};
};

NS_END