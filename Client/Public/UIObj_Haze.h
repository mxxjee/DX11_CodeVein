#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_BitmapFont;
class UIObj_Text;

class UIObj_Haze :
    public UIObject
{
public:
    enum HazeUIEventType{UPDATE_CURRENTHAZE, UPDATE_NEWHAZE,END};
public:
    struct HazeUIEvent
    {
        HazeUIEventType eType;
        void*   pArg = nullptr;

    };
protected:
    explicit UIObj_Haze();
    explicit UIObj_Haze(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Haze(const UIObj_Haze& original);
    virtual ~UIObj_Haze();


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
public:
    static UIObj_Haze* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;

private:
    UIObject*   m_pGlow = nullptr;
    UIObject* m_pNewHaze = nullptr;


    UIObj_BitmapFont*   m_pCurrentHazeText = nullptr;
    UIObj_BitmapFont*   m_pNewHazeText = nullptr;
    UIObj_Text*         m_pPlusText = nullptr;  //∫Œ»£ ui


};
NS_END

