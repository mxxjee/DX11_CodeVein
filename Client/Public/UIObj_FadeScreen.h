#pragma once
#include "UIObject.h"

namespace Engine
{
    class UI_Animation;
    class UIAnimClip;

}
NS_BEGIN(Client)

class UIObj_FadeScreen :
    public UIObject
{
    
public:
    enum class FadeScreenEventType{AUTO_FADE,ENTER_SCENE,EXIT_SCENE,OPEN_MENU,END};
    struct FadeScreenEvent
    {
        FadeScreenEventType eType;
        _float      m_fSecond = 0.f;
        bool        m_bForceLoad = false;       //true인경우 외부에서직접 addlevel 해서로드

        LEVEL        m_iNextLevel;
        wstring      m_UIName = L"";
        UI_MODE eMode = UI_MODE::END;
        
        bool m_bSavePoint = false; //페이드인을 세이브포인트를 통해서 불렀는지 체크용(플레이어 CheckPointEnd진입을 위한)
        function<void()>        m_EndFunc = nullptr;

    };
protected:
    explicit UIObj_FadeScreen();
    explicit UIObj_FadeScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_FadeScreen(const UIObj_FadeScreen& original);
    virtual ~UIObj_FadeScreen();


public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    void        Play_FadeIn();
    void        Play_FadeOut();
public:
    //virtual void        Set_Active(_bool _isActive);

    
public:
    static UIObj_FadeScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;

public:
    virtual void        After_ApplyData();

private:
    Alarm                   m_Alarm;
    FadeScreenEventType     m_CurrentEventType;
    UI_Animation*           m_pUIAnimComp = nullptr;

    LEVEL               m_iNextLevel = LEVEL::END;
    bool                m_bForceLoad = false;
    bool                m_bEnterExit = false;

    UIAnimClip* FadeInClip = nullptr;
    UIAnimClip* FadeOutClip = nullptr;


    _float      m_fSecond = 1.f;
    bool  m_bSavePoint = false;

};

NS_END

