#pragma once
#include "UIObject.h"

NS_BEGIN(Client)

class UIObj_Selector :
    public UIObject
{

protected:
    explicit UIObj_Selector();
    explicit UIObj_Selector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Selector(const UIObj_Selector& original);
    virtual ~UIObj_Selector();
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
    void        MoveTo( _float fY);
public:
    virtual void        After_ApplyData();

public:
    static UIObj_Selector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

private:
    UIObject*        m_pLine = nullptr;

private:
    _float2         m_InitScale;
};
NS_END
