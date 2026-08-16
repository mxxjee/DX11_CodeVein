#pragma once
#include "UIObject.h"

/*선택지 창의 최상위 부모*/

namespace Engine
{
    class UI_WorldComponent;
}
NS_BEGIN(Client)
class NPC;
class UIObj_Npc_ChoiceMenu :
    public UIObject
{
public:
    enum class  NPCMENUEVENTTYPE {INITIALIZE,END_INTERACTION};
    struct NPCMENUEVENT
    {
        NPCMENUEVENTTYPE eType;

        GameObject* m_pTarget = nullptr;
        _float3 TargetOffSet = _float3(-0.3f, 1.5f, 0.f);

    };
protected:
    explicit UIObj_Npc_ChoiceMenu();
    explicit UIObj_Npc_ChoiceMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Npc_ChoiceMenu(const UIObj_Npc_ChoiceMenu& original);
    virtual ~UIObj_Npc_ChoiceMenu();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);


    //virtual HRESULT Ready_Components(void* pArg);


    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);


public:
    virtual void        After_ApplyData();
    virtual void        Set_Active(_bool _isActive);


private:
    void        Change_Target(GameObject* pTarget, _float3 Offset);
    void        Clear_Target();

public:
    static UIObj_Npc_ChoiceMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;

private:
    UI_WorldComponent*  m_pWorldUIComp = nullptr;


};

NS_END