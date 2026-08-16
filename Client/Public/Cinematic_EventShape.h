#pragma once
#include "EventShape.h"

/*파트오브젝트가 될수도있고 그냥 월드상배치ㅏㄱ될수도있고?*/
NS_BEGIN(Client)

class Monster;

class Cinematic_EventShape :
    public EventShape
{
public:
    typedef struct CinematicEventShapeDesc :public EventShape::EventShapeDescription
    {
        _float fRadius = {};
    }CINEMATICEVENTSHAPE_DESC;

    // 몬스터 1마리에 대한 스폰 정보
    struct MonsterSpawnInfo
    {
        _uint iMonsterID;   // 스폰할 몬스터 ID
        _float3 vPosition;  // 스폰 위치
        _float3 vRotation;  // 스폰 회전값
    };

private:
    explicit Cinematic_EventShape();
    explicit Cinematic_EventShape(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Cinematic_EventShape(const Cinematic_EventShape& original);
    virtual ~Cinematic_EventShape();

public:
    HRESULT Initialize_Prototype(LEVEL _level);
    HRESULT Initialize(void* arg);
    _int	Update_Priority(const _float fTimeDelta);
    _int	Update(const _float fTimeDelta);
    _int	Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta);

    virtual HRESULT Ready_Components();

private:
    /*저장 값 및 생성시 넘겨줄값들.*/
    _float m_fRadius = {};

    bool m_bIsTriggered = false;

    vector<CINEMATIC_KEYFRAME> m_vecCinematicKeyFrames; // 키프레임 데이터 모음


public:
    static Cinematic_EventShape* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
    virtual Cinematic_EventShape* Clone(void* arg);

public:
    bool    Is_Collision();
    void    Free() override;
};

NS_END
