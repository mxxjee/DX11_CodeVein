#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_SceneSlot;
class UIObj_SceneSelectMenu :
    public UIObject
{
public:
    enum SelectMenuEvenType{MINUS_IDX, PLUS_IDX};
public:
    struct SceneSlotMenuEvent
    {
        SelectMenuEvenType eType;
    };
protected:
    explicit UIObj_SceneSelectMenu();
    explicit UIObj_SceneSelectMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_SceneSelectMenu(const UIObj_SceneSelectMenu& original);
    virtual ~UIObj_SceneSelectMenu();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);
   
public:
    virtual void        After_ApplyData();
    virtual void        Set_Active(_bool _isActive);

public:
    static UIObj_SceneSelectMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

private:
    void        Change_Idx();
 
public:
    void Free() override;

private:
    int       m_iIdx;//현재 focus되고있는 sceneslot객체번호
    int       m_iMaxIdx;
    vector< UIObj_SceneSlot*>           m_pSceneSlots;
    bool                            m_Init = true;

};
NS_END

