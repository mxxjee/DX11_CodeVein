#include "Engine_Define.h"
#include "ColliderManager.h"

#include "Collider.h"
#include "GameInstance.h"
// #include "EventManager.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::ColliderManager::ColliderManager()
{
}

Engine::ColliderManager::ColliderManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

Engine::ColliderManager::~ColliderManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



////////////////////////////////////////////////////////  객체 준비 함수  ////////////////////////////////////////////////////////
HRESULT Engine::ColliderManager::Initialize()
{
    return S_OK;
}
/*******************************************************  객체 준비 함수  *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::ColliderManager::Update_Priority(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::ColliderManager::Update(const _float fTimeDelta)
{
    // 매 프레임 시작 시 모든 Collider 충돌 상태 초기화
    for (auto& group : m_arrGroups)
    {
        for (auto* pCollider : group)
        {
            if (pCollider->Is_Active())
                pCollider->Set_Collision(false);
        }
    }

    // 충돌 그룹별로 돌면서 전부 검사
    for (const auto& collPair : m_vecCollisionPairs)
    {
        size_t srcIndex = CAST(size_t)(collPair.first);
        size_t dstIndex = CAST(size_t)(collPair.second);

        const auto& srcGroup = m_arrGroups[srcIndex];
        const auto& dstGroup = m_arrGroups[dstIndex];

        for (auto* pSrc : srcGroup)
        {
            if (!pSrc->Is_Active())
                continue;

            for (auto* pDst : dstGroup)
            {
                if (pSrc == pDst)
                    continue;

                if (!pDst->Is_Active())
                    continue;

                // 충돌했으면 세팅
                if (pSrc->Intersects(pDst->Get_Bounding()))
                {
                    pSrc->Set_Collision(true, pDst);
                    pDst->Set_Collision(true, pSrc);
                }
            }
        }
    }

    return 0;
}
#pragma region 이벤트 발행 업데이트(오버헤드 심할것 같아서 폐기)
//_int Engine::ColliderManager::Update(const _float fTimeDelta)
//{
//    /* 등록된 충돌 쌍 순회 */
//    for (const auto& collPair : m_vecCollisionPairs)
//    {
//        // 타입 불일치 짜증나서 size_t로 함
//        size_t srcIndex = CAST(size_t)(collPair.first);
//        size_t dstIndex = CAST(size_t)(collPair.second);
//
//        const auto& srcGroup = m_arrGroups[srcIndex];
//        const auto& dstGroup = m_arrGroups[dstIndex];
//
//        /* src 그룹의 모든 Collider vs dst 그룹의 모든 Collider */
//        for (auto* pSrc : srcGroup)
//        {
//            if (!pSrc->Is_Active())
//                continue;
//
//            for (auto* pDst : dstGroup)
//            {
//                if (pSrc == pDst)
//                    continue;
//
//                if (!pDst->Is_Active())
//                    continue;
//
//                _ullong key = Make_CollisionKey(pSrc, pDst);
//                _bool isPrevColl = (m_setActiveCollisions.count(key) > 0);
//                _bool isCurrColl = Intersect(pSrc, pDst);
//
//                CollisionEvent eventData;
//                eventData.pSrc = pSrc;
//                eventData.pDst = pDst;
//
//                if (isCurrColl && !isPrevColl)
//                {
//                    /* Enter: 이번 프레임에 새로 충돌 */
//                    eventData.isEnter = true;
//                    m_setActiveCollisions.insert(key);
//                }
//                else if (isCurrColl && isPrevColl)
//                {
//                    /* Stay: 충돌 유지 중 */
//                    eventData.isStay = true;
//                }
//                else if (!isCurrColl && isPrevColl)
//                {
//                    /* Exit: 충돌 끝남 */
//                    eventData.isExit = true;
//                    m_setActiveCollisions.erase(key);
//                }
//                else
//                {
//                    /* 충돌 없음, 이벤트 발행 안함 */
//                    continue;
//                }
//
//                // EventManager를 통해 충돌 이벤트 발행
//                m_pGameInstance->Publish<CollisionEvent>(eventData);
//                // 또는 Publish_Stack으로 지연 발행
//                // m_pGameInstance->Publish_Stack<CollisionEvent>(eventData);
//            }
//        }
//    }
//
//    return 0;
//}

#pragma endregion 이벤트 발행 업데이트(오버헤드 심할것 같아서 폐기)
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::ColliderManager::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ColliderManager::Render(const _float fTimeDelta)
{
    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



////////////////////////////////////////////////////////  등록/해제 함수 ////////////////////////////////////////////////////////
void Engine::ColliderManager::Register(COLLISION_GROUP _groupTag, Collider* _collider)
{
    m_arrGroups[CAST(size_t)(_groupTag)].push_back(_collider);

    Safe_AddRef(_collider);
    _collider->Set_Group(_groupTag);
}

void Engine::ColliderManager::Unregister(COLLISION_GROUP _groupTag, Collider* _collider)
{
    auto& colliders = m_arrGroups[CAST(size_t)(_groupTag)];

    for (size_t i = 0; i < colliders.size(); ++i)
    {
        if (colliders[i] == _collider)
        {
            // swap-and-pop으로 제거시 시간복잡도 O(1) 으로 단축
            colliders[i] = colliders.back();
            colliders.pop_back();
            Safe_Release(_collider);
            return;
        }
    }
}

void Engine::ColliderManager::Add_CollisionPair(COLLISION_GROUP _srcGroup, COLLISION_GROUP _dstGroup)
{
    for (const auto& pair : m_vecCollisionPairs)
    {
        if (pair.first == _srcGroup && pair.second == _dstGroup)
            return;
    }

    m_vecCollisionPairs.push_back({ _srcGroup, _dstGroup });
}

void Engine::ColliderManager::Remove_CollisionPair(COLLISION_GROUP _srcGroup, COLLISION_GROUP _dstGroup)
{
    for (auto iter = m_vecCollisionPairs.begin(); iter != m_vecCollisionPairs.end(); ++iter)
    {
        if (iter->first == _srcGroup && iter->second == _dstGroup)
        {
            m_vecCollisionPairs.erase(iter);
            return;
        }
    }
}

void Engine::ColliderManager::Clear()
{
    for (auto& group : m_arrGroups)
    {
        for (auto& collider : group)
        {
            Safe_Release(collider);
        }
        group.clear();
    }

    m_vecCollisionPairs.clear();
    m_setActiveCollisions.clear();
}
/*******************************************************  등록/해제 함수 *******************************************************/



////////////////////////////////////////////////////////  충돌 판정 검사 함수 ////////////////////////////////////////////////////////
_bool Engine::ColliderManager::Intersect(Collider* _src, Collider* _dst)
{
    return _src->Get_Bounding()->Intersects(_dst->Get_Bounding());
}
/*******************************************************  충돌 판정 검사 함수  *******************************************************/



////////////////////////////////////////////////////////  헬퍼 함수  ////////////////////////////////////////////////////////
_ullong Engine::ColliderManager::Make_CollisionKey(Collider* _pA, Collider* _pB)
{
    /* 두 포인터를 조합해서 유니크 키 생성 */
    /* 순서 무관하게 같은 키를 만들기 위해 정렬 */
    _ullong a = RCAST(_ullong)(_pA);
    _ullong b = RCAST(_ullong)(_pB);

    if (a > b)
        swap(a, b);

    // 해시 충돌 방지를 위해 비트 믹싱 적용
    auto hashMix = [](_ullong _val) -> _ullong
        {
            _val ^= (_val >> 33);
            _val *= 0xFF51AFD7ED558CCD;
            _val ^= (_val >> 33);
            _val *= 0xC4CEB9FE1A85EC53;
            _val ^= (_val >> 33);
            return _val;
        };

    // 솔직히 뭔지 모르겠다 그냥 둘이 충돌중인지 확인용이라고 생각하자
    return hashMix(a) ^ (hashMix(b) * 0x9E3779B97F4A7C15);
}
/*******************************************************  헬퍼 함수  *******************************************************/



////////////////////////////////////////////////////////  생성자 호출 함수  ////////////////////////////////////////////////////////
ColliderManager* Engine::ColliderManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    ColliderManager* pInstance = new ColliderManager(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(), L"ColliderManager 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::ColliderManager::Free()
{
    __super::Free();

    Clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
/*******************************************************  객체 반환 함수  *******************************************************/