#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_BossDead :
    public UIObject
{
public:
    struct BossDeadUIEvent
    {
        _float m_fDeadSecond = 5.f; //NÃÊµÚ¿¡ ²¨Áú°Å´Ï
    };
protected:
    explicit UIObj_BossDead();
    explicit UIObj_BossDead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_BossDead(const UIObj_BossDead& original);
    virtual ~UIObj_BossDead();

    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update(const _float fTimeDelta) override;
public:
    void Free() override;


public:
    static UIObj_BossDead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

    Alarm           m_DeadUIAlarm;      //NÃÊÈÄ ²¨Áü Á¤ÀÇ
};
NS_END
