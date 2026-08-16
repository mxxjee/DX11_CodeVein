#pragma once
#include "UIObject.h"

namespace Engine
{
    class UI_WorldComponent;
}
NS_BEGIN(Client)

class UIObj_Ending :
    public UIObject
{
private:
    explicit UIObj_Ending();
    explicit UIObj_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Ending(const UIObj_Ending& original);
	virtual ~UIObj_Ending();


public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);
    _int Update_Priority(const _float fTimeDelta);
    _int Update(const _float fTimeDelta);
    _int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

public:
    static UIObj_Ending* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    virtual void        After_ApplyData();
    virtual void        Set_Active(_bool _isActive);


public:
    void Free() override;

public:
    void        Set_InitPos(_float3 vInitPos) { m_fInitWorldPos = vInitPos; }
private:
	UI_WorldComponent* m_pWorldComponent = nullptr;
    _float3 m_fInitWorldPos;
    _float3 m_fTargetPos;
    _float m_fSpeed=0.3f;
    
    _float3 m_fSpawnOffSet = _float3(0.f, 0.f, 0.f);
    _float       m_fDelay = 0.3f;

private:
	Alarm   m_DelayAlarm;
    bool    m_bMove = false;


};
NS_END
