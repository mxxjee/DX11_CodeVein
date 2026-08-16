#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_LoadingCube :
    public UIObject
{
protected:
    explicit UIObj_LoadingCube();
    explicit UIObj_LoadingCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_LoadingCube(const UIObj_LoadingCube& original);
    virtual ~UIObj_LoadingCube();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    static UIObj_LoadingCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;
public:
    virtual void        After_ApplyData();

private:
    vector<UIObject*>       m_Parts;

};

NS_END
