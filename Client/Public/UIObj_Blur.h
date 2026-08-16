#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_Blur :
    public UIObject
{
protected:
    explicit UIObj_Blur();
    explicit UIObj_Blur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Blur(const UIObj_Blur& original);
    virtual ~UIObj_Blur();


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
    static UIObj_Blur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
public:
    virtual void        After_ApplyData();

public:
    void Free() override;


};
NS_END
