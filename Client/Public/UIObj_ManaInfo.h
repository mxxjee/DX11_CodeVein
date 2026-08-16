#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_BitmapFont;


class UIObj_ManaInfo :
    public UIObject
{

protected:
    explicit UIObj_ManaInfo();
    explicit UIObj_ManaInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_ManaInfo(const UIObj_ManaInfo& original);
    virtual ~UIObj_ManaInfo();


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
public:
    static UIObj_ManaInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;

private:
    UIObj_BitmapFont* m_pCurrentManaText = nullptr;
    UIObj_BitmapFont* m_pMaxManaText = nullptr;


};
NS_END
