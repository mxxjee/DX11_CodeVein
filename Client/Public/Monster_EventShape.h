#pragma once
#include "EventShape.h"

/*파트오브젝트가 될수도있고 그냥 월드상배치ㅏㄱ될수도있고?*/
NS_BEGIN(Client)

class Monster;

class Monster_EventShape :
    public EventShape
{
public:
    typedef struct MonsterEventShapeDesc :public EventShape::EventShapeDescription
    {
        class Monster* pOwner = nullptr;
        _float3     vOffSet = _float3(0.f, 0.f, 0.f);
        COLLIDER eColliderType = COLLIDER::AABB;
        _float3  Extents = _float3(1.f, 1.f, 1.f);

        ordered_json jExtraData;
    }MONSTEREVENTSHAPE_DESC;

    // 몬스터 1마리에 대한 스폰 정보
    struct MonsterSpawnInfo
    {
        _uint iMonsterID;   // 스폰할 몬스터 ID
        _float3 vPosition;  // 스폰 위치
        _float3 vRotation;  // 스폰 회전값
    };

private:
    explicit Monster_EventShape();
    explicit Monster_EventShape(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Monster_EventShape(const Monster_EventShape& original);
    virtual ~Monster_EventShape();

public:
    HRESULT Initialize_Prototype();
    HRESULT Initialize(void* arg);
    _int	Update_Priority(const _float fTimeDelta);
    _int	Update(const _float fTimeDelta);
    _int	Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta);

    virtual HRESULT Ready_Components();

public:
    static Monster_EventShape* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual Monster_EventShape* Clone(void* arg);

    virtual ordered_json Get_ExtraData();
    virtual void Set_ExtraData(const ordered_json& j);

    void Add_SpawnInfo(const MonsterSpawnInfo& _info) { m_vecSpawnList.push_back(_info); }
    vector<MonsterSpawnInfo>& Get_SpawnList() { return m_vecSpawnList; }

public:
    bool    Is_Collision();
    void    Free() override;

private:
    void Execute_Spawn();

private:
    /*저장 값 및 생성시 넘겨줄값들.*/
    class Monster* m_pOwner = nullptr;
    bool    m_bCollision = false;
    _float3      m_vOffSet = _float3(0.f, 0.f, 0.f);
    _float3      m_Extents;
    vector<MonsterSpawnInfo> m_vecSpawnList;
    class Player* m_pPlayer = nullptr;

    bool m_bIsTriggered = false;
};
NS_END
