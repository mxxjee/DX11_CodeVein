#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_FocusMenu;

class UIObj_Window_Title :
    public UIObject
{
protected:
    explicit UIObj_Window_Title();
    explicit UIObj_Window_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Window_Title(const UIObj_Window_Title& original);
    virtual ~UIObj_Window_Title();
public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);
public:
    void Free() override;
public:
    virtual void        After_ApplyData();

public:
    static UIObj_Window_Title* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
    

private:
    bool        m_bStartInit = true;
    UIObject*    m_pPressAnyKey = nullptr;

private:
                    //0,1,2
    _uint           m_iTitleButtonIdx = 2;
    _uint           m_iCurrentButtonIdx = 0;

    vector<UIObject*>       m_subMenus;

private:
    UIObj_FocusMenu* m_pFocusMenu = nullptr;

    UIObject* m_ConfigButton = nullptr;
};
NS_END

