#pragma once
#include "UIObj_Text.h"
namespace Engine
{
    class UI_Progress;

}
NS_BEGIN(Client)


class UIObj_PopUp_ItemGet :
    public UIObject
{
    
public:
    enum class PopUpState{SHOW,HIDE,READY,END};
    struct PopUpDesc
    {
        UIObject*   pRoot = nullptr;
        UI_Progress* pRootProgress = nullptr;

        UIObject*   m_pPanel = nullptr;
        UIObj_Text* m_pItemName = nullptr;
        UIObj_Text* m_pItemCount = nullptr;

        void Visible_All(bool b)
        {
            pRoot->Set_Visible(b);
            m_pPanel->Set_Visible(b);
            m_pItemName->Set_Visible(b);
            m_pItemCount->Set_Visible(b);

        }

        void Active_All(bool b)
        {
            pRoot->Set_Active(b);
            m_pPanel->Set_Active(b);
            m_pItemName->Set_Active(b);
            m_pItemCount->Set_Active(b);

        }

        void Set_Alpha(float f)
        {
            pRoot->Set_Alpha(f);
            m_pPanel->Set_Alpha(f);
            m_pItemName->Set_Alpha(f);
            m_pItemCount->Set_Alpha(f);
        }
    };

    struct PopUpEvent
    {
        
        ItemInfo* pInfo = nullptr;      //팝업에 띄울 아이템 정보


    };

    
protected:
    explicit UIObj_PopUp_ItemGet();
    explicit UIObj_PopUp_ItemGet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_PopUp_ItemGet(const UIObj_PopUp_ItemGet& original);
    virtual ~UIObj_PopUp_ItemGet();
    
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

    void                Change_State(PopUpState eState);
    void                Update_State(const _float& fTimeDelta);
public:
    static UIObj_PopUp_ItemGet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
public:
    void Free() override;

private:
    void        Set_IconInfo(ItemInfo*  pItemInfo);
    void        Reset();

private:
    /*자식 캐싱*/
    UIObject*               m_pIconBase = nullptr;
    UIObject*               m_pItemIcon = nullptr;

    UIObject*               m_pIconEffect = nullptr;
    

    UIObject*               m_pScrollBar = nullptr;
    UI_Progress*            m_pScrollProgress = nullptr;

    PopUpDesc               popupDesc;

    

private:
    PopUpState              m_ePopUpState = PopUpState::READY;
    _float                  m_fScrollBarProgress=1.f;
    string                  ShowAnimKey = "OnShow";
    string                  HideAnimKey = "OnHide";

    Alarm               m_DescScrollAlarm;//
    Alarm               m_DescProgressAlarm;//언제 펴지는효과날건지 시간세는것
    Alarm               m_HideAlarm;//꺼지는시간

    Alarm               m_ReadyAlarm;


};
NS_END

