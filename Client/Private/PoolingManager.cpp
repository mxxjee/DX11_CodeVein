#include "Client_Define.h"
#include "PoolingManager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ParticleSystem.h"

PoolingManager* PoolingManager::m_pInstance = nullptr;

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::PoolingManager::PoolingManager()
{
}

Client::PoolingManager::~PoolingManager()
{
    Free();
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::PoolingManager::Initialize()
{
    if (m_pGameInstance)
        return S_OK;

    m_pGameInstance = GameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);

    /* DEAD_EVENT 구독: 죽은 오브젝트를 자동으로 풀에 반납 */
    m_pGameInstance->Subscribe<DEAD_EVENT>([this](const DEAD_EVENT& _event) {
        if(_event.bDeadObject == true)
            Release_DeadObject(_event);

        });
        
    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 풀 등록 ////////////////////////////////////////////////////////
HRESULT Client::PoolingManager::Register(
    POOL_ID _ePoolID, _uint _count,
    _uint _prototypeLevel, const wstring& _prototypeTag,
    _uint _levelID, const wstring& _layerName,
    void* _arg, _uint _argSize, POOL_POLICY _ePolicy)
{
    _uint poolIndex = _uint(_ePoolID);

    /* 범위 검사 */
    if (poolIndex >= _uint(POOL_ID::END))
        return E_FAIL;

    PoolBucket& bucket = m_arrPools[poolIndex];

    /* 이미 등록된 풀이면 스킵 */
    if (bucket.bRegistered)
        return S_OK;

    bucket.iPrototypeLevel = _prototypeLevel;
    bucket.strPrototypeTag = _prototypeTag;
    bucket.iLevelID = _levelID;
    bucket.strLayerName = _layerName;
    bucket.ePolicy = _ePolicy;
    bucket.bRegistered = true;
    bucket.ePoolID = _ePoolID;

    /* DESC 템플릿 복사 (나중에 Grow할 때 재사용) */
    if (_arg != nullptr && _argSize > 0)
    {
        bucket.pArgTemplate = malloc(_argSize);
        memcpy(bucket.pArgTemplate, _arg, _argSize);
        bucket.iArgSize = _argSize;
    }

    /* Pre-warm: _count개 Clone 후 비활성화 */
    bucket.vecSlots.reserve(_count);

    for (_uint i = 0; i < _count; ++i)
    {
        GameObject* object = { nullptr };

        CHECK_FAILED(m_pGameInstance->Add_GameObject_To_Layer(
            _prototypeLevel, _prototypeTag,
            _levelID, _layerName,
            &object, _arg), E_FAIL);

        /* 풀 소속 정보 기록 */
        object->Set_PoolInfo(poolIndex, i);

        /* 비활성화 후 슬롯에 저장 */
        object->Set_Active(false);
        bucket.vecSlots.push_back(object);
        bucket.queueFree.push(i);
        Safe_AddRef(object);
    }

    return S_OK;
}

HRESULT Client::PoolingManager::Register_Effect(POOL_ID _ePoolID, ParticleSystem* _pSystem, _uint _count, _uint _levelID, const wstring& _layerName, POOL_POLICY _ePolicy)
{
    _uint poolIndex = _uint(_ePoolID);

    /* 범위 검사 */
    if (poolIndex >= _uint(POOL_ID::END))
        return E_FAIL;

    PoolBucket& bucket = m_arrPools[poolIndex];

    /* 이미 등록된 풀이면 스킵 */
    if (bucket.bRegistered)
        return S_OK;

    bucket.iLevelID = _levelID;
    bucket.pSystem = _pSystem;
    Safe_AddRef(bucket.pSystem);
    bucket.strLayerName = _layerName;
    bucket.ePolicy = _ePolicy;
    bucket.bRegistered = true;
    bucket.ePoolID = _ePoolID;

    /* Pre-warm: _count개 Clone 후 비활성화 */
    bucket.vecSlots.reserve(_count);

    for (_uint i = 0; i < _count; ++i)
    {
        GameObject* object = CAST(GameObject*)(_pSystem)->Clone(nullptr);

        // 이름 인덱스 붙이기
        _wstring wstrName = _pSystem->Get_Name() + to_wstring(i);
        object->Set_Name(wstrName);

        CHECK_FAILED(m_pGameInstance->Add_GameObject(object, _levelID, _layerName), E_FAIL);

        /* 풀 소속 정보 기록 */
        object->Set_PoolInfo(poolIndex, i);

        /* 비활성화 후 슬롯에 저장 */
        object->Set_Active(false);
        bucket.vecSlots.push_back(object);
        bucket.queueFree.push(i);
        Safe_AddRef(object);
    }

    return S_OK;
}
/******************************************************* 풀 등록 *******************************************************/



//////////////////////////////////////////////////////// Acquire ////////////////////////////////////////////////////////
GameObject* Client::PoolingManager::Acquire(POOL_ID _ePoolID, void* _arg)
{
    _uint poolIndex = _uint(_ePoolID);

    if (poolIndex >= _uint(POOL_ID::END))
        return nullptr;

    PoolBucket& bucket = m_arrPools[poolIndex];

    if (!bucket.bRegistered)
        return nullptr;

    _uint slotIndex = {};

    if (bucket.queueFree.empty())
    {
        /* 풀이 꽉 참 -> 포화 정책 처리 */
        slotIndex = Handle_Saturation(bucket);
        if (slotIndex == _uint(-1))
            return nullptr;
    }
    else
    {
        /* 비활성 슬롯에서 꺼냄 */
        slotIndex = bucket.queueFree.front();
        bucket.queueFree.pop();
    }

    GameObject* object = bucket.vecSlots[slotIndex];

    CHECK_NULL_RESULT(object, nullptr);

    /* 활성화 */
    object->OnSpawn(_arg);
    object->Set_Active(true);

    /* 활성 순서 기록 (RECYCLE_OLDEST용) */
    bucket.dequeActiveOrder.push_back(slotIndex);

    /* 통계 갱신 */
    bucket.iActiveCount++;
    if (bucket.iActiveCount > bucket.iHighWatermark)
        bucket.iHighWatermark = bucket.iActiveCount;

    return object;
}

GameObject* Client::PoolingManager::Acquire_Effect(POOL_ID _ePoolID, void* _arg)
{
    _uint poolIndex = _uint(_ePoolID);

    if (poolIndex >= _uint(POOL_ID::END))
        return nullptr;

    PoolBucket& bucket = m_arrPools[poolIndex];

    if (!bucket.bRegistered)
        return nullptr;

    _uint slotIndex = {};

    if (bucket.queueFree.empty())
    {
        /* 풀이 꽉 참 -> 포화 정책 처리 */
        slotIndex = Handle_Saturation(bucket);
        if (slotIndex == _uint(-1))
            return nullptr;
    }
    else
    {
        /* 비활성 슬롯에서 꺼냄 */
        slotIndex = bucket.queueFree.front();
        bucket.queueFree.pop();
    }

    GameObject* object = bucket.vecSlots[slotIndex];

    CHECK_NULL_RESULT(object, nullptr);

    /* 활성 순서 기록 (RECYCLE_OLDEST용) */
    bucket.dequeActiveOrder.push_back(slotIndex);

    /* 통계 갱신 */
    bucket.iActiveCount++;
    if (bucket.iActiveCount > bucket.iHighWatermark)
        bucket.iHighWatermark = bucket.iActiveCount;

    return object;
}
/******************************************************* Acquire *******************************************************/



//////////////////////////////////////////////////////// Release ////////////////////////////////////////////////////////
void Client::PoolingManager::Despawn_Object(GameObject* _object)
{
    if (_object == nullptr || !_object->Is_Pooled())
        return;

    _uint poolIndex = _object->Get_PoolID();
    _uint slotIndex = _object->Get_PoolSlotIndex();

    if (poolIndex >= _uint(POOL_ID::END))
        return;

    PoolBucket& bucket = m_arrPools[poolIndex];

    /* 이미 비활성이면 중복 Release 방지 */
    if (!_object->Is_Active())
        return;

    /* 정리 콜백 호출 */
    _object->OnDespawn();
    _object->Set_Active(false);

    /* 슬롯을 사용 가능 목록에 반납 */
    bucket.queueFree.push(slotIndex);

    /* 활성 순서에서 제거 */
    auto& activeOrder = bucket.dequeActiveOrder;
    for (auto it = activeOrder.begin(); it != activeOrder.end(); ++it)
    {
        if (*it == slotIndex)
        {
            activeOrder.erase(it);
            break;
        }
    }

    /* 통계 갱신 */
    if (bucket.iActiveCount > 0)
        bucket.iActiveCount--;
}

void Client::PoolingManager::Remove_Object(GameObject* _object)
{
    if (_object == nullptr || !_object->Is_Pooled())
        return;

    _uint poolIndex = _object->Get_PoolID();
    _uint slotIndex = _object->Get_PoolSlotIndex();

    if (poolIndex >= _uint(POOL_ID::END))
        return;

    PoolBucket& bucket = m_arrPools[poolIndex];

    /* 활성 상태면 Despawn 먼저 */
    if (_object->Is_Active())
    {
        _object->OnDespawn();
        _object->Set_Active(false);

        /* 활성 순서에서도 제거 */
        auto& activeOrder = bucket.dequeActiveOrder;
        for (auto it = activeOrder.begin(); it != activeOrder.end(); ++it)
        {
            if (*it == slotIndex)
            {
                activeOrder.erase(it);
                break;
            }
        }

        if (bucket.iActiveCount > 0)
            bucket.iActiveCount--;
    }

    /* vecSlots에서 슬롯 제거 */
    bucket.vecSlots[slotIndex] = nullptr;

    /* queueFree 재구성 (중간 제거) */
    queue<_uint> newQueue;
    while (!bucket.queueFree.empty())
    {
        _uint index = bucket.queueFree.front();
        bucket.queueFree.pop();
        if (index != slotIndex)
            newQueue.push(index);
    }
    bucket.queueFree = move(newQueue);

    _object->Set_PoolInfo(-1, -1);

    Safe_Release(_object);
}
/******************************************************* Release *******************************************************/



//////////////////////////////////////////////////////// 포화 정책 처리 ////////////////////////////////////////////////////////
_uint Client::PoolingManager::Handle_Saturation(PoolBucket& _bucket)
{
    switch (_bucket.ePolicy)
    {
    case POOL_POLICY::DENY:
    {
        return _uint(-1);
    }

    case POOL_POLICY::RECYCLE_OLDEST:
    {
        if (_bucket.dequeActiveOrder.empty())
            return _uint(-1);

        _uint oldestSlot = _bucket.dequeActiveOrder.front();
        _bucket.dequeActiveOrder.pop_front();

        GameObject* pOldest = _bucket.vecSlots[oldestSlot];

        /* 강제 Release */
        pOldest->OnDespawn();

        if (_bucket.iActiveCount > 0)
            _bucket.iActiveCount--;

        return oldestSlot;
    }

    case POOL_POLICY::GROW:
    {
        _uint poolID = {};

        /* 기존 슬롯에서 poolID 추출 */
        if (!_bucket.vecSlots.empty())
            poolID = _bucket.vecSlots[0]->Get_PoolID();

        if (FAILED(Grow_Bucket(_bucket, poolID)))
            return _uint(-1);

        _uint newSlot = _bucket.queueFree.front();
        _bucket.queueFree.pop();
        return newSlot;
    }
    }

    return _uint(-1);
}
/******************************************************* 포화 정책 처리 *******************************************************/



//////////////////////////////////////////////////////// 풀 확장 ////////////////////////////////////////////////////////
HRESULT Client::PoolingManager::Grow_Bucket(PoolBucket& _bucket, _uint _poolID)
{
    _uint baseIndex = (_uint)_bucket.vecSlots.size();

    if (_bucket.pSystem != nullptr)
    {
        for (_uint i = 0; i < _bucket.iGrowBatchSize; i++)
        {
            ParticleSystem* pNewSystem = _bucket.pSystem->Clone(nullptr);

            m_pGameInstance->Add_GameObject(pNewSystem, _bucket.iLevelID, _bucket.strLayerName);

            _uint newIndex = baseIndex + i;
            pNewSystem->Set_PoolInfo(_poolID, newIndex);
            pNewSystem->Set_Active(false);
            Safe_AddRef(pNewSystem);

            _bucket.vecSlots.push_back(pNewSystem);
            _bucket.queueFree.push(newIndex);
        }

        return S_OK;
    }

    for (_uint i = 0; i < _bucket.iGrowBatchSize; ++i)
    {
        GameObject* object = { nullptr };

        CHECK_FAILED(m_pGameInstance->Add_GameObject_To_Layer(
            _bucket.iPrototypeLevel, _bucket.strPrototypeTag,
            _bucket.iLevelID, _bucket.strLayerName,
            &object, _bucket.pArgTemplate), E_FAIL);

        _uint newIndex = baseIndex + i;
        object->Set_PoolInfo(_poolID, newIndex);
        object->Set_Active(false);
        Safe_AddRef(object);

        _bucket.vecSlots.push_back(object);
        _bucket.queueFree.push(newIndex);
    }

    return S_OK;
}
/******************************************************* 풀 확장 *******************************************************/



//////////////////////////////////////////////////////// 이벤트 기반 Release ////////////////////////////////////////////////////////
void Client::PoolingManager::Release_DeadObject(const DEAD_EVENT& _event)
{
    if (_event.pDeadObject == nullptr)
        return;

    // DeadEvent 받았을 때 풀링된 오브젝트라면
    if (_event.pDeadObject->Is_Pooled())
    {
        Remove_Object(_event.pDeadObject);
    }
    else
    {
        // 아니면 PoolingManager에 등록 안 된거니까 그냥 패스
        // Safe_Release(_event.pDeadObject); 
    }
}
/******************************************************* 이벤트 기반 Release *******************************************************/



//////////////////////////////////////////////////////// 특정 풀 정리 ////////////////////////////////////////////////////////
void Client::PoolingManager::Clear(POOL_ID _ePoolID)
{
    _uint poolIndex = _uint(_ePoolID);

    if (poolIndex >= _uint(POOL_ID::END))
        return;

    PoolBucket& bucket = m_arrPools[poolIndex];

    if (!bucket.bRegistered)
        return;

    /* 모든 오브젝트 해제 */
    for (_uint i = 0; i < (_uint)bucket.vecSlots.size(); ++i)
    {
        auto* object = bucket.vecSlots[i];
        if (object == nullptr)
            continue;

        if (object->Is_Active())
        {
            object->OnDespawn();
            object->Set_Active(false);
        }

        Safe_Release(object);
        bucket.vecSlots[i] = nullptr;
    }

    bucket.vecSlots.clear(); // 슬롯 벡터 자체도 비움

    /* 큐/덱 초기화 */
    while (!bucket.queueFree.empty())
        bucket.queueFree.pop();
    bucket.dequeActiveOrder.clear();

    bucket.iActiveCount = 0;
    bucket.bRegistered = false; // 재등록 전까지 Acquire 차단

    /* DESC 템플릿 메모리도 해제 */
    if (bucket.pArgTemplate != nullptr) // 완전 해제 시 템플릿도 정리
    {
        free(bucket.pArgTemplate);
        bucket.pArgTemplate = nullptr;
        bucket.iArgSize = 0;
    }

    // ParticleSystem 정리
    if (bucket.pSystem)
        Safe_Release(bucket.pSystem);

}

void Client::PoolingManager::Clear_Range(POOL_ID _begin, POOL_ID _end)
{
    if (m_bIsAllCleared == true)
        return;

    for (_uint i = _uint(_begin); i < _uint(_end); ++i)
    {
        if (m_arrPools[i].bRegistered)
            Clear(POOL_ID(i));
    }
}
/******************************************************* 특정 풀 정리 *******************************************************/



//////////////////////////////////////////////////////// 전체 정리 ////////////////////////////////////////////////////////
void Client::PoolingManager::Clear_All()
{
    for (_uint i = 0; i < _uint(POOL_ID::END); ++i)
    {
        if (m_arrPools[i].bRegistered)
            Clear(POOL_ID(i));
    }
    m_bIsAllCleared = true;
}
/******************************************************* 전체 정리 *******************************************************/



//////////////////////////////////////////////////////// 통계 ////////////////////////////////////////////////////////
_uint Client::PoolingManager::Get_ActiveCount(POOL_ID _ePoolID)
{
    _uint poolIndex = _uint(_ePoolID);

    if (poolIndex >= _uint(POOL_ID::END))
        return 0;

    return m_arrPools[poolIndex].iActiveCount;
}

_uint Client::PoolingManager::Get_HighWatermark(POOL_ID _ePoolID)
{
    _uint poolIndex = _uint(_ePoolID);

    if (poolIndex >= _uint(POOL_ID::END))
        return 0;

    return m_arrPools[poolIndex].iHighWatermark;
}

void Client::PoolingManager::Debug_PrintAll()
{
    for (_uint i = 0; i < _uint(POOL_ID::END); ++i)
    {
        PoolBucket& bucket = m_arrPools[i];

        if (!bucket.bRegistered)
            continue;

        _char buf[256] = {};
        sprintf_s(buf, "[Pool] ID:%u | Total:%u Active:%u Free:%u HW:%u",
            i,
            (_uint)bucket.vecSlots.size(),
            bucket.iActiveCount,
            (_uint)bucket.queueFree.size(),
            bucket.iHighWatermark);

        COUT(buf);
    }
}

UMAP<_wstring, POOL_ID> Client::PoolingManager::Get_CurrentObjectPoolName()
{
    UMAP<_wstring, POOL_ID> umappool;

    // 등록된 pool목록만을 UMAP에 담기
    for (auto& pool : m_arrPools)
    {
        if(pool.bRegistered == false)
            continue;
        if (pool.ePoolID == POOL_ID::END)
            continue;
        if (pool.ePoolID <= POOL_ID::MONSTER_BEGIN || pool.ePoolID >= POOL_ID::MONSTER_END)
            continue;

        _wstring poolName = PoolID_ToWstring(pool.ePoolID);

        umappool.emplace(poolName, pool.ePoolID);
    }

    return umappool;
}

_wstring Client::PoolingManager::PoolID_ToWstring(POOL_ID _poolID)
{
    switch (_poolID)
    {
    case POOL_ID::MONSTER_SLIME: return L"Devil_Slime";
    case POOL_ID::MONSTER_SLAVE_DEVIL: return L"Slave_Devil";
    case POOL_ID::MONSTER_SLAVE_VAMPIRE: return L"Slave_Vampire";
    case POOL_ID::MONSTER_OLIVER: return L"Oliver";
    case POOL_ID::MONSTER_WOLFGHOST: return L"WolfGhost";
    case POOL_ID::MONSTER_GIANT_VAMPIRE: return L"GiantVampire";
    case POOL_ID::MONSTER_MONKEY_DEVIL: return L"Monkey_Devil";
    case POOL_ID::MONSTER_GIANT_WHITEDEVIL: return L"WhiteGiantDevil";
    case POOL_ID::MONSTER_GHOSTKNIGHT: return L"GhostKnight";
    default: return L"";
    }

    return L"";
}
/******************************************************* 통계 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::PoolingManager::Free()
{
    Clear_All();

    /* DESC 템플릿 메모리 해제 */
    for (_uint i = 0; i < _uint(POOL_ID::END); ++i)
    {
        if (m_arrPools[i].pArgTemplate != nullptr)
        {
            free(m_arrPools[i].pArgTemplate);
            m_arrPools[i].pArgTemplate = nullptr;
        }
    }

    Safe_Release(m_pGameInstance);
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/