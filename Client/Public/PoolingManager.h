#pragma once
#include "Client_Define.h"
#include "GameInstance.h"
#include "Base.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class GameInstance;
class GameObject;
NS_END

NS_BEGIN(Client)

class PoolingManager final : public Base
{
    /* 아오 IMPLEMENT_SINGLETON 왜 안 되냐고 짜증나네 */
public:
    static PoolingManager* Get_Instance() {
        if (m_pInstance == nullptr)
        {
            m_pInstance = new PoolingManager();
        }
        return m_pInstance;
    }
    static void DestroyInstance()
    {
        if (m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = nullptr;
        }
    }

private:
    explicit PoolingManager();
    explicit PoolingManager(const PoolingManager&) = delete;
    PoolingManager& operator=(const PoolingManager&) = delete;
    virtual ~PoolingManager();

public:
    HRESULT Initialize();

    /*==========================================================*/
    /*                      풀 등록                             */
    /*==========================================================*/

    /* 풀 등록 + Pre-warm */
    /*
    *   _ePoolID        : POOL_ID (배열 인덱스로 직접 사용)
    *   _count          : 미리 생성할 개수
    *   _prototypeLevel : 프로토타입이 등록된 레벨 ID
    *   _prototypeTag   : 프로토타입 태그
    *   _levelID        : 오브젝트가 배치될 레벨 ID
    *   _layerName      : 오브젝트가 들어갈 레이어 이름
    *   _arg            : Clone 시 넘길 DESC (nullptr 가능)
    *   _argSize        : DESC 크기 (추가 Clone용 복사에 필요)
    *   _ePolicy        : 포화 정책 (풀이 꽉 찼을 때 어떻게 할 것인가)
    */
    HRESULT Register(POOL_ID _ePoolID, _uint _count,
        _uint _prototypeLevel, const wstring& _prototypeTag,
        _uint _levelID, const wstring& _layerName,
        void* _arg = nullptr, _uint _argSize = 0,
        POOL_POLICY _ePolicy = POOL_POLICY::RECYCLE_OLDEST);


    /* 이펙트 등록용 함수 */
    HRESULT Register_Effect(POOL_ID _ePoolID, ParticleSystem* _pSystem,
        _uint _count, _uint _levelID, const wstring& _layerName,
        POOL_POLICY _ePolicy = POOL_POLICY::GROW);

    /*==========================================================*/
    /*                   Acquire / Release                      */
    /*==========================================================*/

    /* 풀에서 오브젝트 꺼내기 */
    GameObject* Acquire(POOL_ID _ePoolID, void* _arg = nullptr);
    GameObject* Acquire_Effect(POOL_ID _ePoolID, void* _arg = nullptr);

    /* 오브젝트를 풀에 반납 */
    void Despawn_Object(GameObject* _object);
    /* 오브젝트를 풀에서 완전히 제거 */
    void Remove_Object(GameObject* _object);

    /*==========================================================*/
    /*                      풀 정리                             */
    /*==========================================================*/

    /* 특정 풀의 활성 오브젝트 전부 Release */
    void Clear(POOL_ID _ePoolID);

    /* 특정 풀의 활성 오브젝트 전부 Release(범위) */
    void Clear_Range(POOL_ID _begin, POOL_ID _end);

    /* 전체 풀 정리 (레벨 전환 시) */
    void Clear_All();

    /*==========================================================*/
    /*                    디버깅/통계                           */
    /*==========================================================*/

    _uint Get_ActiveCount(POOL_ID _ePoolID);
    _uint Get_HighWatermark(POOL_ID _ePoolID);
    void Debug_PrintAll();
    
    /*==========================================================*/
    /*                    Get함수                               */
    /*==========================================================*/
    UMAP<_wstring, POOL_ID> Get_CurrentObjectPoolName();
    _wstring PoolID_ToWstring(POOL_ID _poolID);
    

private:
    /*==========================================================*/
    /*                    내부 함수                             */
    /*==========================================================*/

    /* 포화 정책 처리 */
    _uint Handle_Saturation(PoolBucket& _bucket);

    /* PoolBucket에 오브젝트 N개 추가 Clone */
    HRESULT Grow_Bucket(PoolBucket& _bucket, _uint _poolID);

    /* 이벤트 기반 Release */
    void Release_DeadObject(const DEAD_EVENT& _event);

private:
    /* 전체 풀 저장소 (POOL_ID를 인덱스로 직접 접근) */
    array<PoolBucket, _uint(POOL_ID::END)> m_arrPools;

    /* 이벤트 핸들 */
    vector<EventHandle> m_vecEventHandle;

    GameInstance* m_pGameInstance = { nullptr };

    static PoolingManager* m_pInstance;

    _bool m_bIsAllCleared = false;

public:
    void Free() override final;
};

NS_END