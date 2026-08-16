#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)
class UI_BitmapText;
NS_END

NS_BEGIN(Client)

class UIObj_BitmapFont :
    public UIObject
{
protected:
    explicit UIObj_BitmapFont();
    explicit UIObj_BitmapFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_BitmapFont(const UIObj_BitmapFont& original);
    virtual ~UIObj_BitmapFont();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    void        Set_ValueFloat(_float* pCurrent) { m_fValue = pCurrent;}
    void        Set_Text(wstring str);
public:
    virtual void        After_ApplyData();
    static UIObj_BitmapFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;

private:
    UI_BitmapText* m_pBitmapFont = nullptr;

    _float* m_fValue = nullptr;

};

NS_END