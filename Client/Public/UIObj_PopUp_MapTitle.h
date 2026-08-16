#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_Text;


class UIObj_PopUp_MapTitle :
    public UIObject
{
public:
    //enum class MapTitleUIEventType{};
    struct MapTitleUIEvent
    {
        wstring m_Text;
          
        _float      m_fDeactiveSecond = 8.f;    //유지시간

    };
protected:
    explicit UIObj_PopUp_MapTitle();
    explicit UIObj_PopUp_MapTitle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_PopUp_MapTitle(const UIObj_PopUp_MapTitle& original);
    virtual ~UIObj_PopUp_MapTitle();

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
    static UIObj_PopUp_MapTitle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
public:
    void Free() override;

private:
    UI_Progress* m_LineProgress = nullptr;
    UIObject* m_pLogo = nullptr;
    UIObject* m_pLine = nullptr;
    UIObj_Text* m_pText = nullptr;

    UIObject* m_pGlowTexture = nullptr;

private:
    Alarm           m_DeActiveAlarm;
    Alarm           m_ProgressAlarm;


    _float m_fCurrent = 0.f;
    _float m_fMax = 1.f;

};
NS_END

