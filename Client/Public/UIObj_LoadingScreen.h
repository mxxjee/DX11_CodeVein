#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_Text;

class UIObj_LoadingScreen :
    public UIObject
{
public:
    enum LoadingScreenEventType{DEACTIVE,ACTIVE,UPDATE_TEXTS,END};
    struct LoadingScreenEvent
    {
        LoadingScreenEventType eType;
        float       m_fSecond = 0.f;        //0.f인경우 즉시!!

    };


protected:
    explicit UIObj_LoadingScreen();
    explicit UIObj_LoadingScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_LoadingScreen(const UIObj_LoadingScreen& original);
    virtual ~UIObj_LoadingScreen();


public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    //virtual void        Set_Active(_bool _isActive);

public:
    static UIObj_LoadingScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;
public:
    virtual void        After_ApplyData();
    virtual void        Set_Active(_bool _isActive);
private:
    void        Parsing_LoadingDatas();
    void        Update_Texts();



private:
    Alarm           m_Alarm_To_ToggleActive;

private:
    UIObj_Text* m_pTitleText = nullptr;
    UIObj_Text* m_pDescText = nullptr;


private:
    vector<wstring>     m_wstrTitleTexts;
    vector<wstring>     m_wstrLocalString;

};
NS_END
