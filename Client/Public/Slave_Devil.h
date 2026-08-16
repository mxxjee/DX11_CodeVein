#pragma once
#include "Client_Define.h"
#include "Monster.h"

NS_BEGIN(Engine)
class TrailEffect;
NS_END

NS_BEGIN(Client)

class Slave_Devil :
    public Monster
{
public:
    enum ANIMINDEX
    {
        IDLE,
        DEATH,

        JUMP_ATTACK,//그냥점어택
        
        NORMAL1,//콤보1
        NORMAL2,//콤보2
        NORMAL3,//콤보3

        SPECIAL_MELEE,//세번찍는 강한공격(ATTACKSPECIAL)

        RUN_F,
        WALK_F, WALK_L, WALK_R,

        TURN180L, TURN180R, TURN90_L, TURN90_R,

        BACKSTAB_N, DRAPESUCK_B, DRAPESUCK_B_S, DRAPESUCK_F, DRAPESUCK_F_S,

        DOWN_S, DOWN_P,

        DAMAGE_FL, DAMAGE_FR, DAMAGE_BL, DAMAGE_BR,//데미지 바얗ㅇ에 따른 애니ㅔㅁ이션
        DAMAGE_STRONG_F, DAMAGE_STRONG_B,

        ANIMINDEX_END
    };
private:
    explicit Slave_Devil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Slave_Devil(const Slave_Devil& original);
    virtual ~Slave_Devil() = default;

public:
    virtual HRESULT Initialize_Prototype(LEVEL _level) override;
    virtual HRESULT Initialize(void* _arg) override;
    _int Update_Parallel(const _float fTimeDelta) override final;
    _int Update_Priority(const _float fTimeDelta) override final;

    virtual _int Update(const _float fTimeDelta) override;
    virtual _int Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;
    virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;
private:
    virtual HRESULT Ready_Components() override;
    virtual HRESULT Ready_PartObjects();
    HRESULT Ready_Controller(); //피직스 세팅함수
    HRESULT Ready_PhysXEvent();
    virtual HRESULT Ready_States() override;
    virtual HRESULT Ready_Events() override;
    HRESULT Ready_Value();


private:
    // 디버깅 하려고 override뺀것
    void OnSpawn(void* _arg) override;
    void OnDespawn() override;
    void Update_Trail();

private:
    // Sword Trail 관련 변수
    vector<TrailEffect*>		m_vecTrailEffects;
    _bool						m_bTrailActive = {};

public:
    static Slave_Devil* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
    virtual GameObject* Clone(void* pArg) override;
    virtual void Free() override;



};

NS_END