#pragma once
#include "Monster.h"

class BossBase : public Monster
{
protected:
    explicit BossBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit BossBase(const BossBase& original);
    virtual ~BossBase() = default;

public:
    virtual HRESULT Initialize(void* _arg) override;
    virtual _int Update(const _float fTimeDelta) override;

protected:
    virtual HRESULT Ready_Components() override;
    virtual void Check_Phase() {};
    virtual void Change_Phase(_uint iNextPhase) {};

protected:
    _uint    m_iCurrentPhase = 1;
    _float   m_fMaxGroggy = 100.f;
    _float   m_fCurrentGroggy = 100.f;
    _bool    m_bIsSuperArmor = true;

public:
    virtual void Free() override;
};
