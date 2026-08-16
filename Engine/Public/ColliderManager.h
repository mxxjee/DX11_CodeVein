#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Collider;
class EventManager;
class GameInstance;

/* 충돌 이벤트 구조체 */
struct CollisionEvent
{
    Collider* pSrc = { nullptr };   // 충돌을 건 쪽
    Collider* pDst = { nullptr };   // 충돌을 당한 쪽
    _bool isEnter = { false };      // 이번 프레임에 새로 충돌 시작
    _bool isStay = { false };       // 충돌 유지 중
    _bool isExit = { false };       // 충돌 끝남
};

class ColliderManager final : public Base
{
private:
    explicit ColliderManager();
    explicit ColliderManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~ColliderManager();

public:
    HRESULT Initialize();
    _int    Update_Priority(const _float fTimeDelta);
    _int    Update(const _float fTimeDelta);
    _int    Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta);

public:
    /* Collider 등록/해제 */
    /* _groupTag로 충돌 그룹 분류 (예: "PLAYER_BODY", "EVENT_POINT" 등) */
    void Register(COLLISION_GROUP _groupTag, Collider* _pCollider);
    void Unregister(COLLISION_GROUP _groupTag, Collider* _pCollider);

    /* 어떤 그룹끼리 충돌 검사할지 등록 */
    /* 예: Add_CollisionPair(L"PLAYER_BODY", L"EVENT_POINT") */
    /* 레벨의 Initialize시점에 등록 */
    void Add_CollisionPair(COLLISION_GROUP _srcGroup, COLLISION_GROUP _dstGroup);
    void Remove_CollisionPair(COLLISION_GROUP _srcGroup, COLLISION_GROUP _dstGroup);

    /* 전체 초기화 (레벨 전환 시) */
    void Clear();

private:
    /* 실제 충돌 판정 */
    _bool Intersect(Collider* _src, Collider* _dst);

    /* 충돌 상태 관리 키 생성 (두 Collider 포인터 조합) */
    /* 두 콜라이더가 현재 충돌중인지 판별하기 위해서 만든거 */
    _ullong Make_CollisionKey(Collider* _pA, Collider* _pB);

private:
    static constexpr size_t GROUP_COUNT = CAST(size_t)(COLLISION_GROUP::END);

    /* 그룹별 Collider 목록 */
    /* Key: 그룹 태그, Value: 해당 그룹의 Collider들 */
    array<vector<Collider*>, GROUP_COUNT> m_arrGroups;

    /* 충돌 검사 쌍 목록 */
    /* first: src 그룹, second: dst 그룹 */
    vector<pair<COLLISION_GROUP, COLLISION_GROUP>> m_vecCollisionPairs;

    /* 이전 프레임 충돌 상태 (Enter/Stay/Exit 판정용) */
    /* Key: 두 Collider 포인터 조합 해시, Value: 이전 프레임 충돌 여부 */
    unordered_set<_ullong> m_setActiveCollisions;

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    GameInstance* m_pGameInstance = { nullptr };

public:
    static ColliderManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
    void Free() override final;
};

NS_END