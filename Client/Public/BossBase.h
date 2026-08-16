#pragma once
#include "Monster.h"

NS_BEGIN(Client)

class BossBase : public Monster
{
public:
    enum BOSS_STATE
    {
        BOSS_INIT = Monster::MON_END,   //보스이름, 뭐 처리등 연출용? 무적용? 
        PHASE_CHANGE,
        GROGGY,
        BOSS_END,//16
    };

protected:
    explicit BossBase();
    explicit BossBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit BossBase(const BossBase& original);
    virtual ~BossBase() = default;

public:
    virtual HRESULT Initialize(void* _arg) override;
    virtual _int Update(const _float fTimeDelta) override;
    virtual _uint Select_NextPattern() override { return 0; }

protected:
    virtual HRESULT Ready_Components() override;
    virtual void Check_Phase() {};
    virtual void Change_Phase(_uint iNextPhase) {};

    virtual HRESULT Ready_States() override;


public:
    const wstring& Get_ApperUIName_KR() { return m_ApperUIName_Kr; }
    const wstring& Get_ApperUIName_Eng() { return m_ApperUIName_Eng; }

    virtual void   OnDead();
protected:
    _uint m_iPhase = 1;

public:
    virtual void Free() override;


protected:
    wstring         m_ApperUIName_Kr = L"올리버 콜린스";
    wstring         m_ApperUIName_Eng = L"Oliver Collins";


    Alarm           m_DeadUIApearAlarm; //N초후 등장 정의


};

NS_END