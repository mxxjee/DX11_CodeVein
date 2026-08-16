#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)
class UI_Text;
NS_END


NS_BEGIN(Client)
class UIObj_Text :
    public UIObject
{
public:
    enum class TEXTTYPE{SYSTEM,WARNING,DEFAULT,END};
    struct SYSTEMTEXTEVENT
    {
        wstring Text;
        TEXTTYPE eType = TEXTTYPE::SYSTEM;
        _float  m_fTime = 3.f;      //이 메세지가 표시될시간
    };
protected:
    explicit UIObj_Text();
    explicit UIObj_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Text(const UIObj_Text& original);
    virtual ~UIObj_Text();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    void        Set_Text(wstring str);
    void        Set_Spacing(float f);
    void        Set_Color(_float4 vColor);


public:
    wstring     Get_Text() { return m_wstrText; }
public:
    virtual void        After_ApplyData();
    static UIObj_Text* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;

private:
    UI_Text* m_pText = nullptr;
    wstring     m_wstrText = L"";

    TEXTTYPE        m_eTextType = TEXTTYPE::DEFAULT;
    Alarm           m_Alarm;

};

NS_END
