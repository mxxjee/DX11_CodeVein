#pragma once
#include "UIObject.h"

namespace Engine
{
    class UI_Progress;

}

NS_BEGIN(Client)
class UIObj_Text;

class UIObj_BossName :
    public UIObject
{
public:
    struct BossNameUIEVent
    {
        wstring BossName_Korean = L"";
        wstring BossName_English = L"";

        _float Korean_Spacing = 44.7f;
        _float English_Spacing = 33.58f;

        _float2 English_LocalPos = _float2(-17.9f, 0.f);
        _float2 Korean_LocalPos = _float2(-12.3f, 18.5f);

        bool bActive = true;
        BOSS_TYPE    m_eBossType = BOSS_TYPE::OLIVER;

    }; 

 
protected:
    explicit UIObj_BossName();
    explicit UIObj_BossName(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_BossName(const UIObj_BossName& original);
    virtual ~UIObj_BossName();


public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    virtual void        Set_Active(_bool _isActive);

public:
    static UIObj_BossName* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    virtual void        After_ApplyData();

private:
    void    Reset();

public:
    void Free() override;
private:
    /*ÀÚ½Ä Ä³½Ì*/
    UIObject* m_pNameBack = nullptr;
    UIObject* m_pLine = nullptr;
    UI_Progress* m_pLineProgress = nullptr;


    UIObject* m_pLogo = nullptr;


    UIObject* m_pVerticalLine = nullptr;
    UI_Progress* m_pVerticleProgress = nullptr;

    UIObject* m_pLeft = nullptr;
    UIObject* m_pRight = nullptr;


    UIObj_Text*               m_pText_English=nullptr;
    UIObj_Text*                 m_pText_Korean = nullptr;


private:
    Alarm   VerticalLineActive;
    Alarm   LineActive;
    Alarm   TextActive;
    Alarm   HideAlarm;
    Alarm   ActiveFalseAlarm;

    Alarm   m_SoundAlarm;
private:
    string      m_ShowAnim = "OnShow";
    string      m_HideAnim = "OnHide";


    bool        m_bAfterShow = false;


    BOSS_TYPE       m_eTargetBossType = BOSS_TYPE::END;

};
NS_END

